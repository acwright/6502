# DB Control

A web-based control panel for the 6502 DB Emulator running on Teensy 4.1. This NuxtJS application provides a modern interface to monitor and control your 6502 computer system over the network.

## Overview

DB Control is a single-page application (SPA) that communicates with the DB Emulator firmware via HTTP API. It provides real-time monitoring of CPU state, memory inspection, storage management, and emulator control.

### Features

- **Real-time CPU Monitoring**: View CPU registers (PC, SP, A, X, Y, Status flags)
- **Memory Inspector**: Browse and analyze RAM/ROM contents
- **Storage Management**: Load ROMs, cartridges, and programs from SD card
- **Emulator Control**: Start/stop, step, reset, and configure the emulator
- **Network Discovery**: Automatic connection via mDNS (6502.local)

## Prerequisites

### Hardware Requirements

- **Teensy 4.1** with DB Emulator firmware installed
- **Ethernet connection** between Teensy and your computer/network
- **SD card** (optional, for loading ROMs/programs)

### Software Requirements

- **Node.js** 18.x or higher
- **npm**, **pnpm**, **yarn**, or **bun** package manager
- Modern web browser (Chrome, Firefox, Safari, Edge)

## Installation

1. **Clone or navigate to the project directory**:
   ```bash
   cd /path/to/6502/Firmware/DB\ Control
   ```

2. **Install dependencies**:
   ```bash
   # Using npm
   npm install

   # Using pnpm
   pnpm install

   # Using yarn
   yarn install

   # Using bun
   bun install
   ```

## Configuration

### Teensy Setup

1. **Flash the DB Emulator firmware** to your Teensy 4.1 (see `../DB Emulator/README.md`)

2. **Connect Ethernet cable** between Teensy and your network

3. **Power on the Teensy** and wait for network initialization

4. **Verify network connection**:
   - The firmware initializes mDNS with hostname `6502.local`
   - Web server runs on port 80
   - Serial output will display IP address on startup

### Network Configuration

The application connects to the Teensy via the hostname `6502.local` by default. This uses mDNS for automatic discovery.

**Alternative: Direct IP Connection**

If mDNS doesn't work on your network, you can modify the connection to use a direct IP address:

1. Find the Teensy's IP address from Serial monitor output
2. Update the API calls in `app/app.vue` to use the IP instead of `6502.local`

## Running the Application

### Development Mode

Start the development server with hot-reload:

```bash
# Using npm
npm run dev

# Using pnpm
pnpm dev

# Using yarn
yarn dev

# Using bun
bun run dev
```

The application will be available at `http://localhost:3000`

**Note**: The dev server includes:
- Hot Module Replacement (HMR)
- Vue DevTools integration
- TypeScript type checking
- Auto-import of components and composables

### Production Build

Build the application for production deployment:

```bash
# Using npm
npm run build

# Using pnpm
pnpm build

# Using yarn
yarn build

# Using bun
bun run build
```

Preview the production build locally:

```bash
# Using npm
npm run preview

# Using pnpm
pnpm preview

# Using yarn
yarn preview

# Using bun
bun run preview
```

## Usage

### Initial Connection

1. **Ensure Teensy is powered and connected** to the network
2. **Start the DB Control application** (development or production mode)
3. **Open your browser** to `http://localhost:3000`
4. The app will automatically attempt to connect to `6502.local`

If connection fails:
- Check that Teensy is powered on and on the same network
- Verify the Teensy's IP address via Serial monitor
- Try pinging `6502.local` from your terminal
- Check firewall settings

### Application Interface

The interface is divided into several sections:

#### Header
- Displays connection status
- Shows emulator version and IP address
- Provides quick access to control commands

#### CPU Panel
- Real-time CPU register values
- Program Counter (PC)
- Stack Pointer (SP)
- Accumulator (A), X, and Y registers
- Status flags (N, V, B, D, I, Z, C)

#### Memory Panel
- RAM/ROM inspector
- Hexadecimal memory dump
- Address navigation
- Formatted/raw view options

#### Storage Panel
- Browse ROMs, Cartridges, and Programs
- Load files from SD card
- Paginated file listing
- Enable/disable memory regions

#### Emulator Controls
- **Run/Stop**: Toggle emulator execution
- **Step**: Execute single instruction
- **Reset**: Hardware reset
- **Tick**: Execute single clock cycle
- **Frequency**: Adjust clock speed (+/-)
- **Snapshot**: Save RAM state to SD card

## API Endpoints

The Teensy firmware exposes the following HTTP endpoints:

- `GET /` - HTML interface (embedded in firmware)
- `GET /info` - System information and CPU state
- `GET /memory?target={ram|rom}&page={n}` - Memory dump
- `GET /storage?target={rom|cart|program}&page={n}` - File listing
- `GET /load?target={rom|cart|program}&filename={name}` - Load file
- `GET /control?command={cmd}` - Execute control command

## Troubleshooting

### Connection Issues

**Problem**: "Connecting..." screen persists

**Solutions**:
- Verify Teensy is powered and Ethernet is connected
- Check Serial monitor for IP address assignment
- Try accessing `http://6502.local/info` directly in browser
- Restart Teensy and refresh the application
- Check that mDNS is supported on your network

### Network Discovery Fails

**Problem**: Cannot resolve `6502.local`

**Solutions**:
- Use direct IP address instead (check Serial output)
- Install Bonjour/mDNS support on your system
  - **Windows**: Install Bonjour Print Services
  - **Linux**: Install Avahi daemon (`avahi-daemon`)
  - **macOS**: Built-in support

### Application Won't Start

**Problem**: Development server fails to start

**Solutions**:
- Ensure Node.js 18+ is installed (`node --version`)
- Delete `node_modules` and `.nuxt` directories
- Reinstall dependencies (`npm install`)
- Check for port conflicts (port 3000)

### Memory/Storage Not Loading

**Problem**: Empty or error when viewing memory/files

**Solutions**:
- Verify SD card is inserted and formatted (FAT32)
- Check that ROMs/Carts/Programs directories exist
- Ensure files are properly named (.bin extension)
- Try toggling RAM/ROM/Cart enable switches

## Development

### Project Structure

```
DB Control/
├── app/
│   ├── app.vue              # Main application component
│   ├── assets/              # CSS and static assets
│   ├── components/          # Vue components
│   │   ├── cpu.vue          # CPU state display
│   │   ├── emulator.vue     # Emulator controls
│   │   ├── memory.vue       # Memory inspector
│   │   ├── storage.vue      # File browser
│   │   └── layout/          # Layout components
│   ├── composables/         # Vue composables
│   └── types/               # TypeScript type definitions
├── server/
│   └── api/                 # API route handlers
│       ├── control.ts       # Control commands proxy
│       ├── info.ts          # System info proxy
│       ├── load.ts          # File loading proxy
│       ├── memory.ts        # Memory access proxy
│       └── storage.ts       # Storage listing proxy
├── public/                  # Static files
├── nuxt.config.ts          # Nuxt configuration
├── package.json            # Dependencies
└── tsconfig.json           # TypeScript configuration
```

### Key Technologies

- **Nuxt 4**: Vue.js meta-framework with SSR disabled (SPA mode)
- **@nuxt/ui**: UI component library
- **TypeScript**: Type-safe development
- **Tailwind CSS**: Utility-first styling

### Adding Features

1. **New Component**: Add to `app/components/`
2. **New API Route**: Add to `server/api/`
3. **New Composable**: Add to `app/composables/`
4. **State Management**: Use `useState()` or Pinia stores

## Related Documentation

- [DB Emulator Firmware](../DB%20Emulator/README.md)
- [Nuxt Documentation](https://nuxt.com/docs)
- [Vue 3 Documentation](https://vuejs.org/)
- [Nuxt UI Components](https://ui.nuxt.com/)

## License

See LICENSE file in project root.

## Credits

Created by A.C. Wright © 2024
