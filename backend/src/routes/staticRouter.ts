import fs from "node:fs";
import send from "koa-send";
import path from "node:path";
import Router from "@koa/router";

const router = new Router();

const publicDir = path.join(process.cwd(), "public");

const indexPage = (() => {
    try {
        return fs.readFileSync(process.cwd() + "/public/index.html", "utf-8");
    } catch {
        return undefined;
    }
})();
router.all("/(.*)", async (ctx, next) => {
    let flag: any = undefined;
    let p = ctx.path;
    if (ctx.path == "/camera" || ctx.path == "/flight-attitude") {
        p += ".html";
    }
    try {
        flag = await send(ctx, p, {
            maxAge: 10 * 24 * 60 * 60,
            root: publicDir
        });
    } catch (err) {
        if ((err as { status: number }).status !== 404) {
            throw err;
        }
    }
    if (!flag) {
        if (indexPage != null) {
            ctx.body = indexPage;
            ctx.type = "text/html";
        } else {
            await next();
        }
    }
});

export default router;
