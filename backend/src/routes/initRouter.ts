import Koa from "koa";
import apiRouter from "./apiRouter";
import staticRouter from "./staticRouter";

export default function initRouter(app: Koa) {
    app.use(apiRouter.routes()).use(apiRouter.allowedMethods());
    app.use(staticRouter.routes()).use(staticRouter.allowedMethods());
}
