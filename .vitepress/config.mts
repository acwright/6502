import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  srcDir: "Documentation",
  title: "8-Bit Retro Computer",
  description: "8-Bit Retro Computer",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config

    logo: '/6502.svg',

    nav: [
      { text: 'Docs', link: '/introduction/what-is-6502' }
    ],

    sidebar: [
      {
        text: 'Introduction',
        collapsed: false,
        items: [
          { text: 'What is the 6502 Project?', link: '/introduction/what-is-6502' },
        ]
      }
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/acwright/6502' }
    ]
  }
})
