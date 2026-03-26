import { defineConfig } from 'vite';
import { mockDevServerPlugin } from 'vite-plugin-mock-dev-server';

export default defineConfig({
  plugins: [
    mockDevServerPlugin(),
  ],
  server: {
    proxy: {
      '/info': { target: '' },
      '/control': { target: '' },
      '/memory': { target: '' },
      '/storage': { target: '' },
      '/load': { target: '' },
      '/keyboard': { target: '' },
    },
  },
  build: {
    rollupOptions: {
      input: {
        main: './index.html',
        'sound-worklet': './src/sound-worklet.js',
      },
      output: {
        // Control the naming of entry chunks (main JS/CSS files)
        entryFileNames: `assets/[name].js`,

        // Control the naming of non-entry chunks (dynamically imported modules)
        chunkFileNames: `assets/[name].js`,

        // Control the naming of assets (images, fonts, etc.)
        assetFileNames: `assets/[name].[ext]`,
      },
    },
  },
});