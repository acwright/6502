// ---------------------------------------------------------------------------
// TMS9918A Canvas Renderer
// ---------------------------------------------------------------------------

// TMS9918A 16-color palette [r, g, b]
const PALETTE = [
  [0,   0,   0  ],  // 0  Transparent
  [0,   0,   0  ],  // 1  Black
  [33,  200, 66 ],  // 2  Medium Green
  [94,  220, 120],  // 3  Light Green
  [84,  85,  237],  // 4  Dark Blue
  [125, 118, 252],  // 5  Light Blue
  [212, 82,  77 ],  // 6  Dark Red
  [66,  235, 245],  // 7  Cyan
  [252, 85,  84 ],  // 8  Medium Red
  [255, 121, 120],  // 9  Light Red
  [212, 193, 84 ],  // 10 Dark Yellow
  [230, 206, 128],  // 11 Light Yellow
  [33,  176, 59 ],  // 12 Dark Green
  [201, 91,  186],  // 13 Magenta
  [204, 204, 204],  // 14 Gray
  [255, 255, 255],  // 15 White
];

const ACTIVE_W = 256;
const ACTIVE_H = 192;
const BORDER_X = 32;   // left/right border pixels (active 256 + 64 = 320 total)
const BORDER_Y = 24;   // top/bottom border pixels (active 192 + 48 = 240 total)
const CANVAS_W = ACTIVE_W + BORDER_X * 2;  // 320
const CANVAS_H = ACTIVE_H + BORDER_Y * 2;  // 240

let canvas = null;
let ctx = null;
let imageData = null;
let pixels = null;
let animId = null;

const vram = new Uint8Array(16384);
const registers = new Uint8Array(8);
let vramAddr = 0;

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

export function init(canvasEl) {
  canvas = canvasEl;
  canvas.width  = CANVAS_W;
  canvas.height = CANVAS_H;
  ctx = canvas.getContext('2d');
  imageData = ctx.createImageData(CANVAS_W, CANVAS_H);
  pixels = imageData.data;

  if (!animId) {
    animId = requestAnimationFrame(render);
  }
}

export function onDataWrite(val) {
  vram[vramAddr & 0x3FFF] = val;
  vramAddr = (vramAddr + 1) & 0x3FFF;
}

export function onRegWrite(reg, val) {
  registers[reg & 7] = val;
}

export function onAddrSet(hi, lo) {
  vramAddr = ((hi & 0x3F) << 8) | lo;
}

export function onReset() {
  vram.fill(0);
  registers.fill(0);
  vramAddr = 0;
}

// ---------------------------------------------------------------------------
// Render loop
// ---------------------------------------------------------------------------

function render() {
  if (!ctx) { animId = requestAnimationFrame(render); return; }

  // Determine video mode from register bits
  // R0 bit 1 = M3, R1 bit 3 = M2, R1 bit 4 = M1
  const m1 = (registers[1] >> 4) & 1;
  const m2 = (registers[1] >> 3) & 1;
  const m3 = (registers[0] >> 1) & 1;

  // Clear to backdrop color
  const backdrop = registers[7] & 0x0F;
  fillBackdrop(backdrop);

  if (m1 === 0 && m2 === 0 && m3 === 0) {
    renderGraphicsI();
    renderSprites();
  } else if (m1 === 0 && m2 === 0 && m3 === 1) {
    renderGraphicsII();
    renderSprites();
  } else if (m1 === 1 && m2 === 0 && m3 === 0) {
    renderText();
    // Text mode: sprites not supported by TMS9918A
  } else if (m1 === 0 && m2 === 1 && m3 === 0) {
    renderMulticolor();
    renderSprites();
  }

  ctx.putImageData(imageData, 0, 0);
  animId = requestAnimationFrame(render);
}

// ---------------------------------------------------------------------------
// Backdrop
// ---------------------------------------------------------------------------

function fillBackdrop(colorIndex) {
  const [r, g, b] = PALETTE[colorIndex] || PALETTE[0];
  for (let i = 0; i < CANVAS_W * CANVAS_H; i++) {
    const off = i * 4;
    pixels[off]     = r;
    pixels[off + 1] = g;
    pixels[off + 2] = b;
    pixels[off + 3] = 255;
  }
}

function setPixel(x, y, colorIndex) {
  if (x < 0 || x >= ACTIVE_W || y < 0 || y >= ACTIVE_H) return;
  if (colorIndex === 0) return; // transparent
  const [r, g, b] = PALETTE[colorIndex];
  const off = ((y + BORDER_Y) * CANVAS_W + (x + BORDER_X)) * 4;
  pixels[off]     = r;
  pixels[off + 1] = g;
  pixels[off + 2] = b;
  pixels[off + 3] = 255;
}

// ---------------------------------------------------------------------------
// Graphics I mode — 32×24 tiles, 8×8 each
// ---------------------------------------------------------------------------

function renderGraphicsI() {
  const nameTable    = (registers[2] & 0x0F) << 10;
  const colorTable   = registers[3] << 6;
  const patternTable = (registers[4] & 0x07) << 11;

  for (let tileY = 0; tileY < 24; tileY++) {
    for (let tileX = 0; tileX < 32; tileX++) {
      const nameAddr = nameTable + tileY * 32 + tileX;
      const charCode = vram[nameAddr & 0x3FFF];

      // Color table: 1 byte per 8 characters (group of 8)
      const colorByte = vram[(colorTable + (charCode >> 3)) & 0x3FFF];
      const fg = (colorByte >> 4) & 0x0F;
      const bg = colorByte & 0x0F;

      for (let row = 0; row < 8; row++) {
        const patternByte = vram[(patternTable + charCode * 8 + row) & 0x3FFF];
        for (let col = 0; col < 8; col++) {
          const bit = (patternByte >> (7 - col)) & 1;
          const color = bit ? fg : bg;
          setPixel(tileX * 8 + col, tileY * 8 + row, color);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Graphics II mode — 32×24 tiles, 3 sections of 256 patterns
// ---------------------------------------------------------------------------

function renderGraphicsII() {
  const nameTable    = (registers[2] & 0x0F) << 10;
  const colorTable   = (registers[3] & 0x80) << 6;
  const patternTable = (registers[4] & 0x04) << 11;

  for (let tileY = 0; tileY < 24; tileY++) {
    for (let tileX = 0; tileX < 32; tileX++) {
      const nameAddr = nameTable + tileY * 32 + tileX;
      const charCode = vram[nameAddr & 0x3FFF];

      // Section: top (0-7), middle (8-15), bottom (16-23) rows
      const section = Math.floor(tileY / 8);
      const patternOffset = section * 2048;

      for (let row = 0; row < 8; row++) {
        const patternByte = vram[(patternTable + patternOffset + charCode * 8 + row) & 0x3FFF];
        const colorByte   = vram[(colorTable + patternOffset + charCode * 8 + row) & 0x3FFF];
        const fg = (colorByte >> 4) & 0x0F;
        const bg = colorByte & 0x0F;

        for (let col = 0; col < 8; col++) {
          const bit = (patternByte >> (7 - col)) & 1;
          const color = bit ? fg : bg;
          setPixel(tileX * 8 + col, tileY * 8 + row, color);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Text mode — 40×24 tiles, 6×8 each, rendered centered in 256px
// ---------------------------------------------------------------------------

function renderText() {
  const nameTable    = (registers[2] & 0x0F) << 10;
  const patternTable = (registers[4] & 0x07) << 11;
  const fg = (registers[7] >> 4) & 0x0F;
  const bg = registers[7] & 0x0F;
  const xOffset = 8; // center 240px (40*6) in 256px

  for (let tileY = 0; tileY < 24; tileY++) {
    for (let tileX = 0; tileX < 40; tileX++) {
      const nameAddr = nameTable + tileY * 40 + tileX;
      const charCode = vram[nameAddr & 0x3FFF];

      for (let row = 0; row < 8; row++) {
        const patternByte = vram[(patternTable + charCode * 8 + row) & 0x3FFF];
        for (let col = 0; col < 6; col++) {
          const bit = (patternByte >> (7 - col)) & 1;
          const color = bit ? fg : bg;
          setPixel(xOffset + tileX * 6 + col, tileY * 8 + row, color);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Multicolor mode — 64×48 "fat pixels" (4×4 each)
// ---------------------------------------------------------------------------

function renderMulticolor() {
  const nameTable    = (registers[2] & 0x0F) << 10;
  const patternTable = (registers[4] & 0x07) << 11;

  for (let tileY = 0; tileY < 24; tileY++) {
    for (let tileX = 0; tileX < 32; tileX++) {
      const nameAddr = nameTable + tileY * 32 + tileX;
      const charCode = vram[nameAddr & 0x3FFF];

      for (let row = 0; row < 8; row++) {
        const patternByte = vram[(patternTable + charCode * 8 + row) & 0x3FFF];
        const leftColor  = (patternByte >> 4) & 0x0F;
        const rightColor = patternByte & 0x0F;

        const py = tileY * 8 + row;
        // Each "pixel" is 4×4 pixels on screen
        for (let dy = 0; dy < 1; dy++) {
          for (let dx = 0; dx < 4; dx++) {
            setPixel(tileX * 8 + dx, py, leftColor);
            setPixel(tileX * 8 + 4 + dx, py, rightColor);
          }
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Sprite rendering (up to 32 sprites, max 4 visible per scanline)
// ---------------------------------------------------------------------------

function renderSprites() {
  const spriteAttrTable   = (registers[5] & 0x7F) << 7;
  const spritePatternTable = (registers[6] & 0x07) << 11;
  const size16 = (registers[1] >> 1) & 1;  // 0 = 8×8, 1 = 16×16
  const magnify = registers[1] & 1;         // double size
  const spriteSize = size16 ? 16 : 8;
  const pixelSize = magnify ? 2 : 1;

  // Collect active sprites (stop at Y = 0xD0)
  const sprites = [];
  for (let i = 0; i < 32; i++) {
    const attrAddr = spriteAttrTable + i * 4;
    const y = vram[attrAddr & 0x3FFF];
    if (y === 0xD0) break;
    const x = vram[(attrAddr + 1) & 0x3FFF];
    const pattern = vram[(attrAddr + 2) & 0x3FFF];
    const colorByte = vram[(attrAddr + 3) & 0x3FFF];
    const color = colorByte & 0x0F;
    const earlyClockBit = (colorByte >> 7) & 1;

    sprites.push({
      y: (y + 1) & 0xFF,  // Y is offset by 1
      x: earlyClockBit ? x - 32 : x,
      pattern,
      color,
    });
  }

  // Render in reverse order (sprite 0 has highest priority)
  // Track per-scanline sprite counts
  const scanlineCount = new Uint8Array(CANVAS_H);

  for (let si = sprites.length - 1; si >= 0; si--) {
    const sp = sprites[si];

    for (let row = 0; row < spriteSize; row++) {
      // Determine which pattern row to read
      let patBytes;
      if (size16) {
        // 16×16 uses 4 consecutive 8×8 patterns arranged in columns
        const patIdx = sp.pattern & 0xFC;
        if (row < 8) {
          patBytes = [
            vram[(spritePatternTable + patIdx * 8 + row) & 0x3FFF],
            vram[(spritePatternTable + (patIdx + 2) * 8 + row) & 0x3FFF],
          ];
        } else {
          patBytes = [
            vram[(spritePatternTable + (patIdx + 1) * 8 + (row - 8)) & 0x3FFF],
            vram[(spritePatternTable + (patIdx + 3) * 8 + (row - 8)) & 0x3FFF],
          ];
        }
      } else {
        patBytes = [vram[(spritePatternTable + sp.pattern * 8 + row) & 0x3FFF]];
      }

      for (let py = 0; py < pixelSize; py++) {
        const screenY = sp.y + row * pixelSize + py;
        if (screenY < 0 || screenY >= CANVAS_H) continue;
        if (scanlineCount[screenY] >= 4) continue;

        let counted = false;
        let colOffset = 0;
        for (let bi = 0; bi < patBytes.length; bi++) {
          const byte = patBytes[bi];
          for (let col = 0; col < 8; col++) {
            if ((byte >> (7 - col)) & 1) {
              for (let px = 0; px < pixelSize; px++) {
                setPixel(sp.x + (colOffset + col) * pixelSize + px, screenY, sp.color);
              }
              if (!counted) { counted = true; }
            }
          }
          colOffset += 8;
        }

        if (counted) scanlineCount[screenY]++;
      }
    }
  }
}
