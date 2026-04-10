const std = @import("std");

const c = @cImport({
    @cInclude("GLFW/glfw3.h");
    @cInclude("GL/gl.h");
});

const Plane = struct {
    x: f32 = 0,
    y: f32 = 2,
    z: f32 = 0,
    yaw: f32 = 0,
    speed: f32 = 0.05,
};

fn drawPlane() void {
    c.glBegin(c.GL_TRIANGLES);

    // nose
    c.glColor3f(1, 0, 0);
    c.glVertex3f(0, 0, -1);

    // left wing
    c.glColor3f(0, 1, 0);
    c.glVertex3f(-0.8, 0, 0.5);

    // right wing
    c.glColor3f(0, 0, 1);
    c.glVertex3f(0.8, 0, 0.5);

    c.glEnd();
}

fn drawTerrain() void {

    const size = 70;
    const step: f32 = 1;

    var x: i32 = -size;

    while (x < size) : (x += 1) {

        c.glBegin(c.GL_TRIANGLE_STRIP);

        var z: i32 = -size;

        while (z <= size) : (z += 1) {

            const h1 = @sin(@as(f32, @floatFromInt(x)) * 0.2) *
                @cos(@as(f32, @floatFromInt(z)) * 0.2);

            const h2 = @sin(@as(f32, @floatFromInt(x + 1)) * 0.2) *
                @cos(@as(f32, @floatFromInt(z)) * 0.2);

            c.glColor3f(0.2, 0.7, 0.2);

            c.glVertex3f(
                @as(f32, @floatFromInt(x)) * step,
                h1,
                @as(f32, @floatFromInt(z)) * step,
            );

            c.glVertex3f(
                @as(f32, @floatFromInt(x + 1)) * step,
                h2,
                @as(f32, @floatFromInt(z)) * step,
            );
        }

        c.glEnd();
    }
}

pub fn main() !void {

    if (c.glfwInit() == 0) return;
    defer c.glfwTerminate();

    const window = c.glfwCreateWindow(1920, 1080, "Zig Flight", null, null) orelse return;

    c.glfwMakeContextCurrent(window);

    c.glEnable(c.GL_DEPTH_TEST);

    var plane = Plane{};

    while (c.glfwWindowShouldClose(window) == 0) {

        // controls
        if (c.glfwGetKey(window, c.GLFW_KEY_W) == c.GLFW_PRESS) {
            plane.x += @cos(plane.yaw) * plane.speed;
            plane.z += @sin(plane.yaw) * plane.speed;
        }

        if (c.glfwGetKey(window, c.GLFW_KEY_S) == c.GLFW_PRESS) {
            plane.x -= @cos(plane.yaw) * plane.speed;
            plane.z -= @sin(plane.yaw) * plane.speed;
        }

        if (c.glfwGetKey(window, c.GLFW_KEY_A) == c.GLFW_PRESS)
            plane.yaw -= 0.04;

        if (c.glfwGetKey(window, c.GLFW_KEY_D) == c.GLFW_PRESS)
            plane.yaw += 0.04;

        if (c.glfwGetKey(window, c.GLFW_KEY_SPACE) == c.GLFW_PRESS)
            plane.y += 0.05;

        if (c.glfwGetKey(window, c.GLFW_KEY_LEFT_SHIFT) == c.GLFW_PRESS)
            plane.y -= 0.05;

        // render
        c.glViewport(0, 0, 1200, 800);

        c.glClearColor(0.4, 0.6, 1.0, 1);
        c.glClear(c.GL_COLOR_BUFFER_BIT | c.GL_DEPTH_BUFFER_BIT);

        c.glMatrixMode(c.GL_PROJECTION);
        c.glLoadIdentity();

        c.glFrustum(-1, 1, -0.7, 0.7, 1, 500);

        c.glMatrixMode(c.GL_MODELVIEW);
        c.glLoadIdentity();

        // camera follow
        c.glRotatef(-plane.yaw * 57.2958, 0, 1, 0);
        c.glTranslatef(-plane.x, -plane.y - 2, -plane.z - 6);

        drawTerrain();

        // draw plane
        c.glPushMatrix();

        c.glTranslatef(plane.x, plane.y, plane.z);
        c.glRotatef(plane.yaw * 57.2958, 0, 1, 0);

        drawPlane();

        c.glPopMatrix();

        c.glfwSwapBuffers(window);
        c.glfwPollEvents();
    }
}