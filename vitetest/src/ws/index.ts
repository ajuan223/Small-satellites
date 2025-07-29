import { currentPicture } from "../data/gallery.ts";
import { roll , pitch , yaw } from "../data/posture.ts";
const ws = new WebSocket("ws://localhost:3001");
// type Data={
//     type:"data"|"image",
//     data:any
// }

// {
//     url: string
// }

ws.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);
        if (data.type == "image") {
            currentPicture.value = data.data; // base64
        }
        if (data.type == "data") {
            roll.value = data.roll;
            pitch.value = data.pitch;
            yaw.value = data.yaw;
        }
    } catch{
    }
};
ws.onclose = () => {``
    console.log("WebSocket closed");
};

export default ws;
