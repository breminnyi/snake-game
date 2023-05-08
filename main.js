const CANV_SIZE = 640;
const NUM_CELLS = 20;
const CELL_SIZE = CANV_SIZE / NUM_CELLS;

function toCellColor(x, y) {
    return (x + y) % 2 ? "#181818FF" : "#183018FF";
}

function fillCell(ctx, color, x, y) {
    ctx.fillStyle = color;
    ctx.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
}

function drawGameOver(ctx) {
    ctx.font = `bold 48px sans-serif`;
    ctx.textAlign = "center";
    ctx.fillStyle = "white";
    ctx.fillText("GAME OVER", CANV_SIZE / 2, CANV_SIZE / 2 + 20); // manually chosen offset
}

const canvas = document.getElementById("viewport");
canvas.height = canvas.width = CANV_SIZE;
const ctx = canvas.getContext("2d");

for (let x = 0; x < NUM_CELLS; x++) {
    for (let y = 0; y < NUM_CELLS; y++) {
        fillCell(ctx, toCellColor(x, y), x, y);
    }
}

function platform_draw_cell(x, y, cellType) {
    let color;
    switch(cellType) {
        case 0:
            color = toCellColor(x, y);
            break;
        case 1:
            color = "green";
            break;
        case 2:
            color = "yellow";
            break;
        default:
            throw new Error(`Not supported cell type: ${cellType}`);
    }
    fillCell(ctx, color, x, y);
}

const wasmMemory = new WebAssembly.Memory({ initial: 2 });
let nextFree = 0x10000;

function platform_malloc(size) {
    size = (size + 3) & (~3); // 4-byte aling
    if (nextFree + size > wasmMemory.buffer.byteLength)
        return 0;
    nextFree += size;
    return nextFree - size;
}

WebAssembly.instantiateStreaming(fetch("bin/snake.wasm"), {
    js: {
        mem: wasmMemory
    },
    env: {
        platform_draw_cell,
        platform_render: () => {},
        platform_malloc
    }
}).then(w => {
    w.instance.exports.game_init(NUM_CELLS, NUM_CELLS, new Date().getTime());
    document.addEventListener("keydown", (e) => {
        w.instance.exports.game_handle_key(e.key.charCodeAt());
    });
    let prev = 0;
    function loop(timestamp) {
        if (prev) {
            const diff = Math.round(timestamp - prev);
            // game_update returns 0 on successful advance of game state; otherwise 1
            if (w.instance.exports.game_update(diff)) {
                drawGameOver(ctx);
                return;
            }
        }
        prev = timestamp;
        window.requestAnimationFrame(loop);
    }
    window.requestAnimationFrame(loop);
});
