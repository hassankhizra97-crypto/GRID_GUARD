import react from '@vitejs/plugin-react'
import { defineConfig, loadEnv } from 'vite'

// https://vite.dev/config/
export default defineConfig(({ mode }) => {
  const env = loadEnv(mode, '.', '')
  const allowedHosts = env.ALLOWED_HOSTS
    ? env.ALLOWED_HOSTS.split(',').map((h) => h.trim())
    : ['localhost', '127.0.0.1']

  return {
    plugins: [react()],
    server: {
      port: 5173,
      // Proxy /api requests to the C++ backend
      proxy: {
        '/api': {
          target: env.BACKEND_URL || 'http://localhost:8081',
          changeOrigin: true,
          secure: false,
        },
      },
      allowedHosts,
    },
  }
})
