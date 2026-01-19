export const useNotifications = () => {
  const toast = useToast()
  let alertCount = 0
  let errorCount = 0

  const alert = (title: string, description?: string) => {
    toast.add({
      id: `${alertCount}`,
      title: title,
      description: description
    })
    alertCount++
  }

  const error = (_error: unknown) => {
    toast.add({
      id: `${errorCount}`,
      title: 'An Error Occurred',
      description: _error instanceof Error ? _error.message : String(error),
      color: 'error',
      icon: 'i-heroicons-exclamation-triangle'
    })
    errorCount++
  }

  return { alert, error }
}