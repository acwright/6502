# DB Emulator Web Interface

This directory contains the web-based control interface for the DB Emulator firmware. The application is built with Vite and provides a comprehensive control panel for managing the emulator's state, memory, and storage.

## Overview

The DB Emulator web interface provides a full-featured control panel with:
- **Real-time emulator control** - Run, stop, step, tick, and reset commands
- **CPU state monitoring** - View registers (A, X, Y, PC, SP, Status)
- **Memory viewer** - Browse and inspect RAM and ROM contents
- **Storage management** - View and manage ROM, cartridge, and program files
- **Clock control** - Adjust emulator frequency
- **Live updates** - Real-time status monitoring via REST API

The application is designed to be lightweight and embeddable in the microcontroller's firmware with minimal footprint.

## Prerequisites

- Node.js (v18 or higher recommended)
- npm (comes with Node.js)

## Project Structure

```
html/
├── README                  # This file
├── package.json           # Project dependencies and scripts
├── vite.config.js         # Vite build configuration
├── index.html             # Main HTML entry point
├── src/                   # Source files
│   ├── index.js          # Application logic and API integration
│   ├── index.css         # Styles and layout
│   └── favicon.ico       # Application icon
├── mock/                  # Mock API for development
│   └── api.mock.js       # Mock server responses
├── dist/                  # Production build output (generated)
│   ├── index.html        # Built HTML for embedding
│   └── assets/           # Built JavaScript and CSS assets
└── node_modules/          # npm dependencies (generated)
```

## Installation

Install the development dependencies:

```sh
npm install
```

This will install:
- **vite** - Modern build tool and dev server with HMR (Hot Module Replacement)
- **vite-plugin-mock-dev-server** - Mock API server for local development

## Development Workflow

### Local Development

Start the development server with live reload and mock API:

```sh
npm run dev
```

This launches Vite's development server (typically at `http://localhost:5173`) with:
- Hot module replacement for instant updates
- Mock API server responding to emulator endpoints
- Automatic browser refresh on file changes

Edit files in `src/` or `index.html` and changes will be reflected immediately.

### Mock API Server

During development, the mock server simulates the emulator's REST API endpoints:
- `GET /info` - Emulator status and configuration
- `POST /control` - Control commands (run, stop, step, etc.)
- `GET /memory` - Read memory pages (RAM/ROM)
- `GET /storage` - List storage files
- `POST /load` - Load program/ROM/cartridge files

The mock server maintains stateful responses so controls actually work during development. Mock configuration is in `mock/api.mock.js`.

### Building for Production

Generate optimized and minified files for firmware embedding:

```sh
npm run build
```

This processes all source files and outputs production-ready files to `dist/`. The build:
- Bundles and minifies JavaScript
- Minifies and inlines CSS
- Optimizes HTML structure
- Outputs assets with predictable names (no hashes) for easier embedding
- Reduces file size for efficient firmware storage

### Preview Production Build

Test the production build locally:

```sh
npm run preview
```

This serves the `dist/` directory to verify the built application works correctly before embedding.

## Integration with Firmware

The built files from `dist/` are served by the DB Emulator firmware from the Teensy 4.1's built-in SD card. After building, copy the contents of the `dist/` directory to the `WWW` folder on the SD card.

The typical workflow is:
1. Modify source files in `src/` or `index.html`
2. Test with `npm run dev` using mock API
3. Build with `npm run build`
4. Copy `dist/` contents to the `WWW` folder on the Teensy 4.1 SD card
5. The firmware's web server will serve the files from the SD card

## API Endpoints

The web interface communicates with the firmware via REST API:

| Endpoint   | Method | Description                          |
|------------|--------|--------------------------------------|
| `/info`    | GET    | Get emulator status and configuration |
| `/control` | POST   | Send control commands                |
| `/memory`  | GET    | Read memory page (RAM/ROM)           |
| `/storage` | GET    | List and read storage files          |
| `/load`    | POST   | Load program/ROM/cartridge file      |

See `mock/api.mock.js` for detailed request/response schemas.

## File Size Considerations

The application must remain minimal to fit within the microcontroller's flash memory constraints. Keep the following in mind:
- Vite automatically optimizes and tree-shakes code
- Avoid adding large external libraries or frameworks
- Use native browser APIs when possible
- Monitor the size of `dist/` output after building
- Test gzipped sizes as the firmware may serve compressed content

## Configuration

### Vite Configuration

The `vite.config.js` file controls:
- Build output naming (assets without hash for predictable embedding)
- API proxy configuration for development
- Mock server plugin integration

### Build Customization

Asset naming can be customized in `vite.config.js` under `build.rollupOptions.output` to match firmware embedding requirements.

## License

See the LICENSE file in the root directory of the DB Emulator project.
