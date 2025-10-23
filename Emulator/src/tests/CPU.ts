import test from 'ava'
import { CPU } from '../components/CPU'
import { readFile } from 'fs/promises'

test('CPU can init', t => {
	t.not(new CPU(() => { return 0 }, () => {}), null)
})

test('CPU can reset', t => {
	const cpu = new CPU((address) => {
		if (address == 0xFFFC) {
			return 0x00
		}
		if (address == 0xFFFD) {
			return 0x80
		}
		return 0x0000
	}, () => {})

	t.is(cpu.pc, 0x8000)
	t.is(cpu.a, 0x00)
	t.is(cpu.x, 0x00)
	t.is(cpu.y, 0x00)
	t.is(cpu.sp, 0xFD)
	t.is(cpu.st, 0x00 | CPU.U)
	t.is(cpu.cycles, 7)
})

test('CPU can irq', t => {
	let ram: number[] = [...Array(0x0200)].fill(0x00)
	
	const cpu = new CPU((address) => {
		if (address == 0xFFFC) {
			return 0x00
		}
		if (address == 0xFFFD) {
			return 0x80
		}
		if (address == 0xFFFE) {
			return 0x00
		}
		if (address == 0xFFFF) {
			return 0xA0
		}
		return 0x0000
	}, (address, data) => {
		ram[address] = data
	})

	cpu.st |= CPU.I // Disable interrupts
	cpu.irq()

	t.is(cpu.cycles, 7)

	cpu.st &= ~CPU.I // Enable interrupts
	cpu.irq()

	t.is(cpu.sp, 0xFD - 3)
	t.is(ram[0x01FD], 0x0080)
	t.is(ram[0x01FC], 0x0000)
	t.is(ram[0x01FB], 0x00 | CPU.U | CPU.I)
	t.is(cpu.pc, 0xA000)
	t.is(cpu.cycles, 14)
})

test('CPU can nmi', t => {
	let ram: number[] = [...Array(0x0200)].fill(0x00)
	
	const cpu = new CPU((address) => {
		if (address == 0xFFFC) {
			return 0x00
		}
		if (address == 0xFFFD) {
			return 0x80
		}
		if (address == 0xFFFA) {
			return 0x00
		}
		if (address == 0xFFFB) {
			return 0xA0
		}
		return 0x0000
	}, (address, data) => {
		ram[address] = data
	})

	t.is(cpu.cycles, 7)

	cpu.nmi()

	t.is(cpu.sp, 0xFD - 3)
	t.is(ram[0x01FD], 0x0080)
	t.is(ram[0x01FC], 0x0000)
	t.is(ram[0x01FB], 0x00 | CPU.U | CPU.I)
	t.is(cpu.pc, 0xA000)
	t.is(cpu.cycles, 14)
})

test('CPU can tick', t => {
	const cpu = new CPU((address) => {
		if (address == 0xFFFC) {
			return 0x00
		}
		if (address == 0xFFFD) {
			return 0x80
		}
		if (address == 0x8000) {
			return 0xEA
		}
		return 0x0000
	}, () => {})

	t.is(cpu.cycles, 7)
	t.is(cpu.cyclesRem, 7)

	cpu.tick()
	cpu.tick()
	cpu.tick()
	cpu.tick()
	cpu.tick()
	cpu.tick()
	cpu.tick()

	t.is(cpu.cycles, 7)
	t.is(cpu.cyclesRem, 0)

	cpu.tick()

	t.is(cpu.pc, 0x8001)
	t.is(cpu.cycles, 9)
	t.is(cpu.cyclesRem, 1)

	cpu.tick()

	t.is(cpu.cycles, 9)
	t.is(cpu.cyclesRem, 0)
})

test('CPU can step', t => {
	const cpu = new CPU((address) => {
		if (address == 0xFFFC) {
			return 0x00
		}
		if (address == 0xFFFD) {
			return 0x80
		}
		if (address == 0x8000) {
			return 0xEA
		}
		return 0x0000
	}, () => {})

	t.is(cpu.cycles, 7)
	t.is(cpu.cyclesRem, 7)

	const cyclesExecuted = cpu.step()

	t.is(cyclesExecuted, 2)
	t.is(cpu.cycles, 9)
	t.is(cpu.cyclesRem, 0)
	t.is(cpu.pc, 0x8001)
})

// TODO: Move to using https://github.com/SingleStepTests/65x02/tree/main/6502

// See https://analog-hors.github.io/site/pones-p1/
// for implementing Klaus 6502 tests

// test('CPU can pass Klaus functional test (6502 Functional Test.bin)', async t => {
// 	let ram: number[]
	
// 	try {
// 		ram = Array.from(new Uint8Array(await readFile(__dirname + '/../../../Software/Tests/6502 Functional Test/build/6502 Functional Test.bin')))
// 	} catch (error) {
// 		console.error('Error reading file:', error)
// 	}

// 	const cpu = new CPU((address) => {
// 		return ram[address]
// 	}, (address, data) => {
// 		ram[address] = data
// 	})

// 	t.is(cpu.cycles, 7)
// 	t.is(cpu.cyclesRem, 7)

// 	// Tick off reset cycles
// 	do {
// 		cpu.tick()
// 	} while (cpu.cyclesRem != 0)

// 	t.is(cpu.cycles, 7)
// 	t.is(cpu.cyclesRem, 0)

// 	// Set PC to program start
// 	cpu.pc = 0x0400
	
// 	let prevPC: number = cpu.pc
// 	do {
// 		prevPC = cpu.pc
// 		cpu.step()
// 	} while (cpu.pc != prevPC)

// 	t.is(cpu.pc, 0x3469)
// })