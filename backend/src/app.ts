import Koa from "koa";
import {koaBody} from "koa-body";

import {logger} from "./utils/log";
import websockify from 'koa-websocket';
import * as net from "node:net";
import {initWSServer} from "./socket/ws";
import getSocketServer from "./socket/tcp";
import initRouter from "./routes/initRouter";

async function main() {
    ["SIGTERM", "SIGINT", "SIGQUIT"].forEach(function (sig) {
        process.on(sig, () => {
            console.warn(`${sig} close process signal detected.`);
            processExit();
        });
    });

    // init koa
    const app = websockify(new Koa());
    initWSServer(app);
    initRouter(app)

    app.use(
        koaBody({
            multipart: true,
            formidable: {
                maxFileSize: 512 * 1024 * 1024
            }
        })
    );

    app.listen(3001,"0.0.0.0", () => logger.log("Server running on port 3001"));

    net.createServer(getSocketServer()).listen(3456,"0.0.0.0");
}

async function processExit() {
    logger.info("Exit.");
    process.exit(0);
}

main().then(() => {
    logger.info("Server started.");
});
