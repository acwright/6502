import test from 'ava'
import { Cart } from '../components/Cart'
import { readFile } from 'fs/promises'

test('Cart can init', t => {
	t.not(new Cart(), null)
})

test('Cart can write', t => {
	const cart = new Cart()

	cart.write(0x0200, 0xAA)
	cart.write(0x0400, 0x55)

	t.is(cart.data[0x0200], 0xAA)
	t.is(cart.data[0x0400], 0x55)
})

test('Cart can read', t => {
	const cart = new Cart()

	t.is(cart.read(0x0000), 0x00)
	t.is(cart.read(0x7FFF), 0x00)
})

test('Cart can load', t => {
	const cart = new Cart()

	cart.load([...Array(Cart.SIZE)].fill(0xAA))

	t.is(cart.data[0x0000], 0xAA)
	t.is(cart.data[0x7FFF], 0xAA)

	cart.load([...Array(Cart.SIZE)].fill(0x55))

	t.is(cart.data[0x0000], 0x55)
	t.is(cart.data[0x7FFF], 0x55)

	// Test loading with oversize data fails to load
	cart.load([...Array(Cart.SIZE + 1)].fill(0xEA))

	t.not(cart.data[0x0000], 0xEA)
	t.not(cart.data[0x7FFF], 0xEA)
	t.is(cart.data[0x0000], 0x55)
	t.is(cart.data[0x7FFF], 0x55)
})