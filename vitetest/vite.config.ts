import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

// https://vite.dev/config/
export default defineConfig({
  plugins: [vue()],
    build:{
      rollupOptions:{
          input:{
              "index": "./index.html",
              "camera": "./camera.html",
              "flight-attitude": "./flight-attitude.html",
          }
      }
    }
})
