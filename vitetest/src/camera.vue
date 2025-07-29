<script setup lang="ts">
// eslint-disable no-unused-vars
// eslint-disable vue/no-unused-vars
import { ref } from "vue";
import { currentPicture, gallery } from "./data/gallery.ts";
import verifyNumberInput from "./utils/verifyNumberInput.ts";
import ws from "./ws";

defineProps<{ msg: string }>();

let servo1 = ref("0");
let servo2 = ref("0");

let timerId: number | null = null;

function start() {
    if (timerId !== null) {
        console.log("捕获已在运行中。");
        return;
    }
    timerId = setInterval(() => {
        // 将新图片地址添加到 images 数组的开头
        if (currentPicture.value) {
            gallery.value.unshift(currentPicture.value);
        }
    }, 1000);
    alert("start");
}

function stop() {
    if (timerId !== null) {
        clearInterval(timerId);
        timerId = null;
        console.log("捕获已停止。");
    }
    alert("stop");
}

function clear() {
    gallery.value = [];
    alert("clear");
}

function send1() {
    ws.send(`1 ${servo1.value};`);
}

function send2() {
    ws.send(`2 ${servo2.value};`);
}
</script>

<template>
    <div id="cmain">
        <h1>ESP32 Camera + Servo Control</h1>
        <hr />
        <img v-if="currentPicture" :src="`data:image/jpg;base64,${currentPicture}`" class="image" />
        <div v-else style="background-color: #b1ccc5" class="image"></div>
        <div class="bigbox mt-100">
            <div class="cbox">
                <input type="button" class="button" value="start" @click="start" />
                <input type="button" class="button" value="stop" @click="stop" />
                <input type="button" class="button" value="clear" @click="clear" />
            </div>
        </div>
    </div>
    <hr />
    <div id="smain">
        <h2>servo control</h2>
        <div class="sbbox">
            <div class="sbox">
                <div class="tbox">servo1</div>
                <input
                    type="text"
                    class="text-input"
                    v-model="servo1"
                    @input="servo1 = verifyNumberInput(servo1, 0)"
                    style="width: 50px"
                />
                <input
                    type="button"
                    class="button"
                    value="set"
                    @click="send1"
                    style="padding-left: 5px"
                />
            </div>
            <div class="sbox">
                <div class="tbox">servo2</div>
                <input
                    type="text"
                    class="text-input"
                    v-model="servo2"
                    @input="
                        servo2 = verifyNumberInput(servo2, 0);
                        console.log(servo2);
                    "
                    style="width: 50px"
                />
                <input
                    type="button"
                    class="button"
                    value="set"
                    @click="send2"
                    style="padding-left: 5px"
                />
            </div>
        </div>
    </div>
    <hr />
    <div id="pmain">
        <h2>图库</h2>
        <div class="pbbox">
            <img v-for="image in gallery" :src="`data:image/jpg;base64,${image}`" :alt="image" />
        </div>
    </div>
</template>

<style>
/* #99EEDD*/

body {
    font-family: Arial, sans-serif;
    background-color: #0e1513;
}

h1 {
    color: #dee4e1;
    margin: 0 auto;
    text-align: center;
}

h2 {
    color: #dee4e1;
    margin: 0 auto;
    text-align: center;
}

.text-input {
    border: none;
    height: auto;
    margin-left: 10px;
    padding: 5px;
    border-radius: 5px;
    background-color: #dee4e1;
    color: #343b39;
}

.text-input:focus {
    border: none;
    color: #005046;
}

.button {
    border: none;
    background-color: #9ff2e1;
    color: #005046;
    padding: 5px;
    border-radius: 5px;
}

.button:active {
    background-color: #cde8e1;
    color: #334b46;
}

#cmain {
    display: flex;
    flex-direction: column;
    max-width: 80%;
    margin: 0 auto;
    align-content: center;
}

.image {
    width: 320px;
    height: 240px;
    display: block;
    margin: 0 auto;
}

.bigbox {
    flex-direction: column;
    margin-top: 20px;
    justify-content: center;
    gap: 10px;
    display: flex;
}

.tbox {
    color: #9ff2e1;
    width: 100px;
    text-align: center;
    justify-content: center;
    align-items: center;
}

.cbox {
    display: flex;
    margin-top: auto;
    text-align: center;
    justify-content: center;
    align-items: center;
    gap: 10px;
}

#smain {
    display: flex;
    flex-direction: column;
    max-width: 80%;
    margin: 0 auto;
    align-content: center;
}

.sbbox {
    display: flex;
    flex-direction: column;
    margin: 0 auto;
    justify-content: center;
    gap: 10px;
}

.sbox {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 20px;
}

#pmain {
    display: flex;
    flex-direction: column;
    margin: 0 auto;
    gap: 10px;
}

.pbbox {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
    justify-content: center;
}
</style>
