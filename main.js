const CELL_SIZE = 24;
const NUM_CELLS = 40;
const CANV_SIZE = CELL_SIZE * NUM_CELLS;

function toCellColor(x, y) {
    return (x + y) % 2 ? "#181818FF" : "#183018FF";
}

function fillCell(ctx, color, x, y) {
    ctx.fillStyle = color;
    ctx.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
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
            color = "yellow";
            break;
        case 2:
            color = "red";
            break;
        default:
            throw new Error(`Not supported cell type: ${cellType}`);
    }
    fillCell(ctx, color, x, y);
}

const wasmMemory = new WebAssembly.Memory({
    initial: 10,
    maximum: 100,
});
let mallocNext = 0;

function platform_malloc(size) {
    size = (size + 3) & (~3);
    if (mallocNext + size > wasmMemory.buffer.byteLength)
        return 0;
    mallocNext += size;
    return mallocNext - size;
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
            if (w.instance.exports.game_update(diff)) {
                console.log("GAME OVER");
                return;
            }
        }
        prev = timestamp;
        window.requestAnimationFrame(loop);
    }
    window.requestAnimationFrame(loop);
});
