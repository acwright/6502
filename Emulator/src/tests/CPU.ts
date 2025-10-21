// See https://analog-hors.github.io/site/pones-p1/
// for implementing Klaus 6502 tests

import test from 'ava'
import { CPU } from '../components/CPU'

test('CPU can init', t => {
	t.not(new CPU(() => { return 0 }, () => {}), null)
})