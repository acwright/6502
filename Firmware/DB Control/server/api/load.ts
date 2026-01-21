import { FetchError } from 'ofetch'

export default defineEventHandler(async (event) => {
  const query = getQuery(event)

  if (!query.ipAddress) {
    throw createError({
      statusCode: 400,
      statusMessage: 'IP address must be included',
    })
  }
  if (!query.target) {
    throw createError({
      statusCode: 400,
      statusMessage: 'Target must be included',
    })
  }
  if (!query.filename) {
    throw createError({
      statusCode: 400,
      statusMessage: 'Filename must be included',
    })
  }

  try {
    return await $fetch(`http://${query.ipAddress}/load`, {
      query: {
        target: query.target,
        filename: query.filename
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