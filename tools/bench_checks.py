"""Add live-settings and fractional-hand checks to the official firmware bench.

Executed against actual LVGL objects in the native firmware, over its normal API.
"""
import asyncio


def install(bench, saver_word):
    original = bench.show_saver

    async def checked(self, example, data, theme):
        image = await original(self, example, data, theme)
        if image is None or getattr(self, '_watch_verified', False):
            return image
        self._watch_verified = True
        start = len(self.lines)
        kind = example['kind']

        async def change(options):
            await self.send(saver_word(self.entry, kind, options, data))
            await asyncio.sleep(.15)
            return await self.render('_live-check', keep=False)

        base = {'motion': 'tick', 'date': False, 'invert': False}
        black = await change(base)
        green = await change({**base, 'invert': True})
        assert black.getpixel((0, 0)) != green.getpixel((0, 0)), 'Background option did not update live'
        back = await change(base)
        assert black.tobytes() == back.tobytes(), 'Restoring options did not restore the face'
        dated = await change({**base, 'date': True})
        assert dated.tobytes() != black.tobytes(), 'Date visibility did not update live'
        smaller = await change({**base, 'scale': 50, 'alignment': 'right'})
        assert smaller.tobytes() != black.tobytes(), 'Scale/alignment did not update live'

        # A fixed wall-clock second plus elapsed monotonic time must move hands.
        # Step the wall clock just before sampling so phase is below its 999 ms cap.
        for motion in ('smooth', 'fluid'):
            await change({**base, 'motion': motion})
            await self.call('render_time', epoch=self.now + 1)
            await asyncio.sleep(.06)
            first = await self.render('_frame-first', keep=False)
            await asyncio.sleep(.18)
            later = await self.render('_frame-later', keep=False)
            assert first.tobytes() != later.tobytes(), f'{motion} hands did not move between second ticks'
            await self.call('render_time', epoch=self.now)

        # Switching animation off must stop interpolation immediately.
        first = await change(base)
        await asyncio.sleep(.18)
        later = await self.render('_tick-still', keep=False)
        assert first.tobytes() == later.tobytes(), 'Tick mode continued animating'
        assert not any('on the glass' in line for line in self.lines[start:]), 'Options recreated the saver'
        await change(example.get('options') or {})
        print('watch-clock live settings and smooth hands: ok', flush=True)
        return await self.render('_saver', keep=False)

    bench.show_saver = checked
