import Router from "@koa/router";

const router = new Router({
    prefix: "/api"
});

router.all("/(.*)", async (ctx) => {
    ctx.body = {
        code: 404,
        error: "NotFound"
    };
});

export default router;