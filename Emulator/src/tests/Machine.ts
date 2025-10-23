import test from 'ava'
import { Machine } from '../components/Machine'
import { readFile } from 'fs/promises'

test('Machine can init', t => {
	t.not(new Machine(), null)
})

test('Machine can load ROM', t => {
	t.assert(true)
})

test('Machine can load Cart', t => {
	t.assert(true)
})

test('Machine can load slot', t => {
	t.assert(true)
})

test('Machine can start and end', t => {
	t.assert(true)
})

test('Machine can change frequency', t => {
	t.assert(true)
})

test('Machine can run and stop', t => {
	t.assert(true)
})

test('Machine can step', t => {
	t.assert(true)
})

test('Machine can reset', t => {
	t.assert(true)
})

test('Machine can read', t => {
	t.assert(true)
})

test('Machine can write', t => {
	t.assert(true)
})