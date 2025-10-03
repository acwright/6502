import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "8-Bit Retro Computer",
  description: "8-Bit Retro Computer",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config

    logo: '/6502.svg',

    nav: [
      { text: 'Docs', link: '/introduction/overview' }
    ],

    sidebar: [
      {
        text: 'Introduction',
        collapsed: false,
        items: [
          { text: 'Overview', link: '/introduction/overview' },
          { text: 'Hardware', link: '/introduction/hardware' },
        ]
      }
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/acwright/6502' }
    ]
  }
})
