// Browser preview of the native face. No device actions, network or parent DOM access.
// Filled contours mirror the native glyph data; the OTF is not embedded.
const numeralGlyphs = /* NUMERAL_POINTS */ [];
const canvas = document.querySelector('canvas');
const ctx = canvas.getContext('2d');
let state = { options: {}, size: { width: 480, height: 480 }, now: Date.now() };
let received = performance.now(), last = -Infinity;
const clamp = (v, lo, hi) => Math.min(hi, Math.max(lo, v));
function render(timestamp) {
  requestAnimationFrame(render);
  const o = state.options, interval = 1000;
  if (timestamp - last < interval) return;
  last = timestamp;
  const W = state.size.width, H = state.size.height;
  if (canvas.width !== W || canvas.height !== H) { canvas.width = W; canvas.height = H; }
  const shape = state.shape || {}, compact = shape.look === 'compact';
  const look = (shape.dpi || (compact ? 143 : 170)) / (compact ? 143 : 170);
  const fonts = shape.fonts || {};
  const fontSizes = [fonts.setpoint || Math.round((compact ? 40 : 64) * look),
    fonts.watch_value || Math.round((compact ? 30 : 38) * look), fonts.headline || Math.round((compact ? 22 : 27) * look),
    fonts.label || Math.round((compact ? 16 : 18) * look), fonts.sublabel || Math.round((compact ? 14 : 16) * look)];
  const lineHeight = size => Math.round(size * 1.172);
  const font = (size, weight = 400) => { ctx.font = `${weight} ${size}px WatchRoboto`; ctx.textBaseline = 'top'; };
  const measure = (text, size, weight = 400) => { font(size, weight); return ctx.measureText(text).width; };
  const dpi = shape.dpi || (compact ? 143 : 170);
  const margin = Math.floor((2 * dpi + 12) / 25);
  const date = new Date(Number(state.now) + performance.now() - received);
  const driftAngle = (date.getHours() * 60 + date.getMinutes()) * .7;
  const driftRadius = Math.floor((dpi + 12) / 25);
  const fw = W - 2 * margin, fh = H - 2 * margin;
  const side = Math.min(fw, fh) * clamp(Number(o.scale ?? 100), 50, 100) / 100;
  const u = side / 220;
  const w = o.layout === 'adaptive' ? Math.min(fw * (o.scale ?? 100) / 100, side * 1.65) : side;
  const h = o.layout === 'adaptive' ? Math.min(fh * (o.scale ?? 100) / 100, side * 1.65) : side;
  let x = (fw - w) / 2, y = (fh - h) / 2;
  if (o.alignment === 'left') x = 0;
  if (o.alignment === 'right') x = fw - w;
  if (o.alignment === 'top') y = 0;
  if (o.alignment === 'bottom') y = fh - h;
  const cx = x + w / 2, cy = y + h / 2;
  const colour = { green: '#4caf50', white: '#ffffff', blue: '#009fe3', purple: '#926bc7', amber: '#ffc107' }[o.numerals || 'green'];
  const bg = o.invert ? colour : '#000000', ink = o.invert ? '#000000' : colour;
  const hand = o.invert && o.numerals === 'white' ? '#000000' : '#ffffff';
  ctx.fillStyle = bg; ctx.fillRect(0, 0, W, H);
  ctx.save(); ctx.translate(margin + Math.round(Math.cos(driftAngle) * driftRadius), margin + Math.round(Math.sin(driftAngle) * driftRadius));
  const label = (text, lx, ly, fs, weight = 400) => {
    font(fs, weight); ctx.fillStyle = ink;
    // Roboto's line box includes ascent above its cap height, as the LVGL labels do.
    ctx.fillText(text, lx, ly + fs * .08);
  };
  const numeralBoxes = numeralGlyphs.map((glyph, i) => {
    const height = (i === 0 ? 74 : 82) * u, width = glyph.width * height;
    let gx = x + (w - width) / 2, gy = y + (h - height) / 2;
    if (i === 0) gy = y + 4 * u;
    if (i === 1) gx = x + w - 4 * u - width;
    if (i === 2) gy = y + h - 4 * u - height;
    if (i === 3) gx = x + 4 * u;
    return { x: gx, y: gy, width, height };
  });
  numeralGlyphs.forEach((glyph, i) => {
    const box = numeralBoxes[i];
    ctx.beginPath();
    glyph.contours.forEach(points => {
      points.forEach(([px, py], n) => {
        const xx = Math.round(box.x) + px * box.height, yy = Math.round(box.y) + py * box.height;
        if (n === 0) ctx.moveTo(xx, yy); else ctx.lineTo(xx, yy);
      });
      ctx.closePath();
    });
    ctx.fillStyle = ink; ctx.fill('evenodd');
  });
  const utc = o.timezone === 'utc', get = key => date[`${utc ? 'getUTC' : 'get'}${key}`]();
  const hour = get('Hours'), minute = get('Minutes'), second = get('Seconds');
  const fraction = 0;
  const day = get('Date'), month = get('Month'), year = get('FullYear');
  if (o.date !== false) {
    const right = (o.date_position || 'bottom_left').includes('right'), top = (o.date_position || '').includes('top');
    const text = o.date_format === 'numeric' ? `${year}-${String(month + 1).padStart(2, '0')}-${String(day).padStart(2, '0')}` :
      o.date_format === 'month' ? `${day} ${['JAN','FEB','MAR','APR','MAY','JUN','JUL','AUG','SEP','OCT','NOV','DEC'][month]}` :
      `${['SUN','MON','TUE','WED','THU','FRI','SAT'][get('Day')]} ${day}`;
    const pad = Math.floor(14 * u), room = Math.max(1, w - 2 * pad);
    const number = numeralBoxes[top ? 0 : 2];
    const leftOutline = number.x, rightOutline = number.x + number.width;
    const corner = Math.max(1, right ? x + w - pad - rightOutline : leftOutline - x - pad);
    const choices = fontSizes.slice(1), first = o.date_size === 'small' ? 2 : o.date_size === 'medium' ? 1 : 0;
    const weights = [500, 500, 700, 400];
    let fs = choices[3], weight = 400;
    for (let i = first; i < choices.length; ++i) if (measure(text, choices[i], weights[i]) <= corner) { fs = choices[i]; weight = weights[i]; break; }
    const width = Math.min(room, measure(text, fs, weight)), lh = lineHeight(fs);
    let lx = x + (right ? w - pad - width : pad), ly = y + (top ? pad : h - pad - lh);
    if (o.date_format === 'numeric' || width > corner) {
      const outside = top ? y - lh - 6 * look : y + h + 6 * look;
      const opposite = top ? y + h + 6 * look : y - lh - 6 * look;
      if (outside >= 0 && outside + lh <= fh) ly = outside;
      else if (opposite >= 0 && opposite + lh <= fh) ly = opposite;
      else if (width > corner) lx = clamp(lx + (right ? 1 : -1) * (width - corner + 2 * look), 0, fw - width);
    }
    label(text, lx, ly, fs, weight);
  }
  const ray = (length, tail, degrees, width, colour, dx = 0, dy = 0) => {
    const a = degrees * Math.PI / 180;
    ctx.beginPath(); ctx.moveTo(cx + dx - Math.sin(a) * tail * u, cy + dy + Math.cos(a) * tail * u);
    ctx.lineTo(cx + dx + Math.sin(a) * length * u, cy + dy - Math.cos(a) * length * u);
    ctx.lineWidth = Math.max(1, Math.floor(width * u)); ctx.lineCap = 'round'; ctx.strokeStyle = colour; ctx.stroke();
  };
  const m = minute + (second + fraction) / 60;
  ray(48, 9, (hour % 12 + m / 60) * 30, 7.8, 'rgba(0,0,0,0.1255)', .7 * u, u);
  ray(82, 10, m * 6, 7.8, 'rgba(0,0,0,0.1255)', .7 * u, u);
  if (o.seconds !== false) ray(91, 13, (second + fraction) * 6, 2.45, 'rgba(0,0,0,0.1255)', .7 * u, u);
  ray(48, 9, (hour % 12 + m / 60) * 30, 6.2, hand);
  ray(82, 10, m * 6, 6.2, hand);
  ctx.beginPath(); ctx.arc(cx, cy, Math.max(3, Math.floor(9.2 * u)) / 2, 0, Math.PI * 2);
  ctx.fillStyle = bg; ctx.fill(); ctx.lineWidth = Math.max(1, Math.floor(1.2 * u)); ctx.strokeStyle = hand; ctx.stroke();
  if (o.seconds !== false) ray(91, 13, (second + fraction) * 6, .85, hand);
  ctx.beginPath(); ctx.arc(cx, cy, Math.max(2, Math.floor(3.6 * u)) / 2, 0, Math.PI * 2);
  ctx.fillStyle = hand; ctx.fill(); ctx.restore();
}
window.addEventListener('message', event => {
  if (event.source !== parent || event.data?.type !== 'tessera:preview') return;
  state = event.data; received = performance.now(); last = -Infinity;
});
Promise.all([400, 500, 700].map(weight => document.fonts.load(`${weight} 100px WatchRoboto`))).then(() => {
  parent.postMessage({ type: 'tessera:preview-ready' }, '*');
  requestAnimationFrame(render);
});
