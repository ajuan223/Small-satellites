import { App } from "koa-websocket";
import * as websocket from "ws";
import { tcpClients } from "./tcp";

export const wsClients = new Set<websocket.WebSocket>();

export function initWSServer(app: App) {
    app.ws.use((ctx) => {
        const ws = ctx.websocket;
        console.log("WebSocket connection established");
        wsClients.add(ws);
        ws.send(JSON.stringify({type: "other", connected: true}));

        ws.on("message", (message: Buffer) => {
            tcpClients.forEach(client => {
                // client.emit('message', message);
                client.write(message);
            });
        })

        ws.on("close", () => {
            wsClients.delete(ws);
        });
        ws.on("error", ()=>{
            wsClients.delete(ws);
        })
    });
}
