import test from 'ava'
import { ROM } from '../components/ROM'

test('ROM can init', t => {
	t.not(new ROM(), null)
})

test('ROM can read', t => {
	const rom = new ROM()

	t.is(rom.read(0x0000), 0xEA)
	t.is(rom.read(0x7FFF), 0xEA)
})

test('ROM can load', t => {
	const rom = new ROM()

	rom.load([...Array(ROM.SIZE)].fill(0xAA))

	t.is(rom.data[0x0000], 0xAA)
	t.is(rom.data[0x7FFF], 0xAA)

	rom.load([...Array(ROM.SIZE)].fill(0x55))

	t.is(rom.data[0x0000], 0x55)
	t.is(rom.data[0x7FFF], 0x55)

	// Test loading with oversize data fails to load
	rom.load([...Array(ROM.SIZE + 1)].fill(0xEA))

	t.not(rom.data[0x0000], 0xEA)
	t.not(rom.data[0x7FFF], 0xEA)
	t.is(rom.data[0x0000], 0x55)
	t.is(rom.data[0x7FFF], 0x55)
})