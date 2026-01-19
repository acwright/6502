export default defineEventHandler(async (event) => {
  const query = getQuery(event)

  await $fetch(/*'http://6502.local/control?command='*/ 'http://192.168.0.124/control', {
    query: {
      command: query.command
    }
  })
})