// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  devtools: { enabled: true },
  ssr: false,

  modules: [
    '@nuxt/ui', 
    '@nuxt/image'
  ],

  imports: {
    dirs: ['src/*.ts']
  },

  css: ['~/assets/css/tailwind.css'],

  typescript: {
    strict: true
  },

  app: {
    head: {
      htmlAttrs: {
        lang: 'en'
      },
      title: '6502 Emulator',
      meta: [
        { name: 'description', content: '6502 Emulator' }
      ],
      link: [
        { rel: 'icon', type: 'image/x-icon', href: '/favicon.png' },
        { rel: 'preconnect', href: 'https://fonts.googleapis.com' },
        { rel: 'preconnect', href: 'https://fonts.gstatic.com', crossorigin: 'anonymous' },
        { rel: 'stylesheet', href: 'https://fonts.googleapis.com/css2?family=Bebas+Neue&display=swap' }
      ]
    }
  },

  postcss: {
    plugins: {
      tailwindcss: {},
      autoprefixer: {},
    },
  },

  compatibilityDate: '2024-07-10'
})