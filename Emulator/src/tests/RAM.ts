import test from 'ava'
import { RAM } from '../components/RAM'

test('RAM can init', t => {
	t.not(new RAM(), null)
})

test('ROM can write', t => {
	const ram = new RAM()

	ram.write(0x0200, 0xAA)
	ram.write(0x0400, 0x55)

	t.is(ram.data[0x0200], 0xAA)
	t.is(ram.data[0x0400], 0x55)
})

test('ROM can read', t => {
	const ram = new RAM()

	t.is(ram.read(0x0000), 0x00)
	t.is(ram.read(0x7FFF), 0x00)
})