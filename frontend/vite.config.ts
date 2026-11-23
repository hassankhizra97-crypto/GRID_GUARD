import react from '@vitejs/plugin-react'
import { defineConfig } from 'vite'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    port: 5173,
    // Proxy all /api requests to the C++ backend so the browser
    // never has to deal with CORS — both servers look like same origin.
    proxy: {
      '/api': {
        target: 'http://localhost:8081',
        changeOrigin: true,
        secure: false,
      },
    },
    allowedHosts: ['evergreen-vista-upwind.ngrok-free.dev', 'localhost'],
  },
})
