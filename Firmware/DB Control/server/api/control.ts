import { FetchError } from 'ofetch'

export default defineEventHandler(async (event) => {
  const query = getQuery(event)

  if (!query.ipAddress) { 
    throw createError({
      statusCode: 400,
      statusMessage: 'IP address must be included',
    })
  }
  if (!query.command) { 
    throw createError({
      statusCode: 400,
      statusMessage: 'Command must be included',
    })
  }

  try {
    return await $fetch(`http://${query.ipAddress}/control`, {
      query: {
        command: query.command
      }
    })
  } catch (error) {
    if (error instanceof FetchError) {
      throw createError({
        statusCode: 404,
        statusMessage: error.message,
      })
    } else {
      throw error
    }
  }
})