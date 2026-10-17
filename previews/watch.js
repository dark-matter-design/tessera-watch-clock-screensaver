// Browser preview of the native face. No device actions, network or parent DOM access.
// Colours mirror Tessera theme roles; typography uses the bundled Tessera Roboto fonts.
const canvas = document.querySelector('canvas');
const ctx = canvas.getContext('2d');
let state = { options: {}, size: { width: 480, height: 480 }, now: Date.now() };
let received = performance.now(), last = -Infinity;
const clamp = (v, lo, hi) => Math.min(hi, Math.max(lo, v));
function render(timestamp) {
  requestAnimationFrame(render);
  const o = state.options, interval = o.motion === 'tick' ? 1000 : o.motion === 'fluid' ? 16 : 33;
  if (timestamp - last < interval) return;
  last = timestamp;
  const W = state.size.width, H = state.size.height;
  if (canvas.width !== W || canvas.height !== H) { canvas.width = W; canvas.height = H; }
  const shape = state.shape || {}, compact = shape.look === 'compact';
  const look = (shape.dpi || (compact ? 143 : 170)) / (compact ? 143 : 170);
  const fonts = shape.fonts || {};
  const watchSize = fonts.watch_digits || Math.floor(Math.min(W, H) * .42);
  const fontSizes = [fonts.setpoint || Math.round((compact ? 40 : 64) * look),
    fonts.watch_value || Math.round((compact ? 30 : 38) * look), fonts.headline || Math.round((compact ? 22 : 27) * look),
    fonts.label || Math.round((compact ? 16 : 18) * look), fonts.sublabel || Math.round((compact ? 14 : 16) * look)];
  ctx.font = `${watchSize}px WatchRoboto`;
  const glyphMetrics = [...'12369'].map(digit => ctx.measureText(digit));
  const watchAscent = Math.ceil(Math.max(...glyphMetrics.map(m => m.actualBoundingBoxAscent)));
  const watchDescent = Math.ceil(Math.max(...glyphMetrics.map(m => m.actualBoundingBoxDescent)));
  const lineHeight = size => size === watchSize ? watchAscent + watchDescent : Math.round(size * 1.172);
  const font = (size, weight = 400) => { ctx.font = `${weight} ${size}px WatchRoboto`; ctx.textBaseline = 'top'; };
  const measure = (text, size, weight = 400) => { font(size, weight); return ctx.measureText(text).width; };
  const margin = Math.round(2 * (shape.dpi || 170) / 25.4);
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
  ctx.save(); ctx.translate(margin, margin);
  let size = fontSizes[0];
  for (const candidate of [watchSize, ...fontSizes.slice(0, 4)]) {
    size = candidate;
    if (lineHeight(size) <= h * .42 && measure('12', size) <= w * .60) break;
  }
  const inset = Math.max(1, Math.floor(2 * u)), height = lineHeight(size);
  const label = (text, lx, ly, fs, weight = 400) => {
    font(fs, weight); ctx.fillStyle = ink;
    // Roboto's line box includes ascent above its cap height, as the LVGL labels do.
    if (fs === watchSize) { ctx.textBaseline = 'alphabetic'; ctx.fillText(text, lx, ly + watchAscent); }
    else ctx.fillText(text, lx, ly + fs * .08);
  };
  ['12', '3', '6', '9'].forEach((text, i) => {
    const width = measure(text, size);
    const lx = i === 1 ? x + w - inset - width : i === 3 ? x + inset : cx - width / 2;
    const ly = i === 0 ? y + inset : i === 2 ? y + h - inset - height : cy - height / 2;
    label(text, lx, ly, size);
  });
  const date = new Date(Number(state.now) + performance.now() - received);
  const utc = o.timezone === 'utc', get = key => date[`${utc ? 'getUTC' : 'get'}${key}`]();
  const hour = get('Hours'), minute = get('Minutes'), second = get('Seconds');
  const fraction = o.motion === 'tick' ? 0 : get('Milliseconds') / 1000;
  const day = get('Date'), month = get('Month'), year = get('FullYear');
  if (o.date !== false) {
    const right = (o.date_position || 'bottom_left').includes('right'), top = (o.date_position || '').includes('top');
    const text = o.date_format === 'numeric' ? `${year}-${String(month + 1).padStart(2, '0')}-${String(day).padStart(2, '0')}` :
      o.date_format === 'month' ? `${day} ${['JAN','FEB','MAR','APR','MAY','JUN','JUL','AUG','SEP','OCT','NOV','DEC'][month]}` :
      `${['SUN','MON','TUE','WED','THU','FRI','SAT'][get('Day')]} ${day}`;
    const pad = Math.floor(14 * u), room = Math.max(1, w - 2 * pad);
    const corner = Math.max(1, (w - measure(top ? '12' : '6', size)) / 2 - pad - 4 * look);
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
  const ray = (length, tail, degrees, width, colour) => {
    const a = degrees * Math.PI / 180;
    ctx.beginPath(); ctx.moveTo(cx - Math.sin(a) * tail * u, cy + Math.cos(a) * tail * u);
    ctx.lineTo(cx + Math.sin(a) * length * u, cy - Math.cos(a) * length * u);
    ctx.lineWidth = Math.max(1, Math.floor(width * u)); ctx.lineCap = 'round'; ctx.strokeStyle = colour; ctx.stroke();
  };
  const m = minute + (second + fraction) / 60;
  ray(48, 9, (hour % 12 + m / 60) * 30, 6.2, hand);
  ray(82, 10, m * 6, 6.2, hand);
  ctx.beginPath(); ctx.arc(cx, cy, Math.max(3, Math.floor(9.2 * u)) / 2, 0, Math.PI * 2);
  ctx.fillStyle = bg; ctx.fill(); ctx.lineWidth = Math.max(1, Math.floor(1.2 * u)); ctx.strokeStyle = hand; ctx.stroke();
  if (o.seconds !== false) ray(91, 13, (second + fraction) * 6, .85, o.invert ? '#000000' : hand);
  ctx.beginPath(); ctx.arc(cx, cy, Math.max(2, Math.floor(3.6 * u)) / 2, 0, Math.PI * 2);
  ctx.fillStyle = hand; ctx.fill(); ctx.restore();
}
window.addEventListener('message', event => {
  if (event.source !== parent || event.data?.type !== 'tessera:preview') return;
  state = event.data; received = performance.now(); last = -Infinity;
});
document.fonts.ready.then(() => {
  parent.postMessage({ type: 'tessera:preview-ready' }, '*');
  requestAnimationFrame(render);
});
