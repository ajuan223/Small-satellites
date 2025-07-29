import { Socket } from "node:net";
import { wsClients } from "./ws";

export const tcpClients = new Set<Socket>();

export default function getSocketServer(): (socket: Socket) => void {
    return (socket: Socket) => {
        tcpClients.add(socket);
        //console.log("Client connected");
        let buffer = ""; // 缓冲区
        socket.on("data", (chunk) => {
            //console.log("Receive data:", chunk.toString());
            buffer += chunk.toString("utf8"); // 拼接数据
            if (!buffer.endsWith("}")) {
                //console.log({ buffer, chunk: chunk.toString() });
                return;
            }
            const raw = buffer;
            buffer = "";
            try {
                // const data: any = JSON.parse(raw);
                console.log("receive message");
                // if (data.type == "image") {
                //     fs.writeFile(
                //         process.cwd() + `/images/${Date.now()}.jpg`,
                //         data.data,
                //         {
                //             encoding: "base64"
                //         },
                //         () => {}
                //     );
                // }
                wsClients.forEach((client) => {
                    client.send(raw);
                });
            } catch (e) {
                console.error(e);
            }
        });

        socket.on("close", () => {
            tcpClients.delete(socket);
        });

        socket.on("error", () => {
            tcpClients.delete(socket);
        });
    };
}
