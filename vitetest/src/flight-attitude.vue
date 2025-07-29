<script setup lang="ts">
import { computed } from "vue";
import { roll , pitch , yaw } from "./data/posture.ts";
// document.getElementById("t1").innerHTML = roll.toFixed(2);
// document
//     .getElementById("lline")
//     .style.setProperty("--degree", `rotate(${roll}deg)`);
const llineStyle = computed(() => {
    return {
        transform: `rotate(${roll.value}deg)`
    };
});
// let mt = 20;
// document.getElementById("t2").innerHTML = mt.toFixed(2);
// document
//     .getElementById("bline")
//     .style.setProperty("--mt", `${(mt + 180) / 3.6 - 5}px`);
const blineStyle = computed(() => {
    const marginTopValue = (pitch.value + 180) / 3.6 - 5;
    return {
        marginTop: `${marginTopValue}px`
    };
});
// let yaw = 0;
// document.getElementById("t3").innerHTML = yaw.toFixed(2);
// document
//     .getElementById("circle")
//     .style.setProperty("--degree", `rotate(${360 + yaw}deg)`);
const circleStyle = computed(() => {
    return {
        transform: `rotate(${yaw.value}deg)`
    };
});
</script>

<template>
    <h1>Flight-attitude</h1>
    <hr>
    <div class="container">
        <div class="bigbox">
            <div class="box">
                <div id="lline" :style="llineStyle"></div>
            </div>
            <div class="tbox">
                <div class="text">roll</div>
                <div class="text"><span id="t1">{{roll}}</span><sup>°</sup></div>
            </div>
        </div>

        <div class="bigbox">
            <div class="box">
                <div id="bline" :style="blineStyle"></div>
            </div>
            <div class="tbox">
                <div class="text">pitch</div>
                <div class="text"><span id="t2">{{pitch}}</span><sup>°</sup></div>
            </div>
        </div>

        <div class="bigbox">
            <div class="box">
                <div id="circle" :style="circleStyle">
                    <div id="sline" />
                </div>
            </div>
            <div class="tbox">
                <div class="text">yaw</div>
                <div class="text"><span id="t3">{{yaw}}</span><sup>°</sup></div>
            </div>
        </div>

    </div>
</template>

<style>
body {
    font-family: Arial, sans-serif;
    background-color: #161217;
    /* color: ; */
}
h1 {
    color: #eae0e7;
    margin-left: auto;
    margin-right: auto;
    text-align: center;
}
.container {
    width: 100%;
    margin-top: 20px;
    margin-left: auto;
    margin-right: auto;
    display: flex; /* 启用Flexbox布局 */
    justify-content: space-around; /* 项目均匀分布 */
}
.bigbox {
    display: flex;
    flex-direction: column;
}
.box {
    width: 100px;
    height: 100px;
    background-color: #5f3768;
    border: 2px solid ;
    color: #524153;
    text-align: center;
    margin-top: 2px; /* 垂直居中 */
}
.tbox {
    width: 100px;
    text-align: center;
    margin-top: 10px; /* 垂直居中 */
    display: flex; /* 启用Flexbox布局 */
    flex-direction: column; /* 垂直排列 */
    justify-content: center; /* 垂直居中内容 */
}
#lline {
    width: 80%;
    height: 5px;
    background-color: #5b8eec;
    margin-top: 50%;
    margin-left: auto;
    margin-right: auto;
    transform: var(--degree);
}
#bline {
    width: 40%;
    height: 10px;
    background-color: #5b8eec;
    margin-top: var(--mt);
    margin-left: auto;
    margin-right: auto;
}
#sline {
    width: 2px;
    height: 50%;
    background-color: #5b8eec;
    margin-bottom: 50%;
    margin-left: 50%;
}
#circle {
    width: 100px;
    height: 100px;
    border-radius: 50%;
    background-color: #5f3768;
    border: 1px solid #524153;
    margin: 0 auto; /* 水平居中 */
    transform: var(--degree);
}
.text {
    color: #e8b5ef;
    font-size: 20px;
    text-align: center;
}
</style>