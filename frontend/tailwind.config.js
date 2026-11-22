/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  darkMode: 'class',
  theme: {
    extend: {
      colors: {
        background: '#0B1120',
        sidebar: '#101A2B',
        card: '#151F32',
        border: '#26344A',
        primary: {
          DEFAULT: '#3B82F6',
          hover: '#2563EB',
          light: '#60A5FA',
        },
        operational: '#22C55E',
        fault: '#EF4444',
        warning: '#F59E0B',
        muted: '#94A3B8',
        text: '#F1F5F9'
      },
      boxShadow: {
        'panel': '0 4px 20px -2px rgba(0, 0, 0, 0.45)',
        'glow-blue': '0 0 15px rgba(59, 130, 246, 0.35)',
        'glow-green': '0 0 15px rgba(34, 197, 94, 0.35)',
        'glow-red': '0 0 15px rgba(239, 68, 68, 0.35)',
      }
    },
  },
  plugins: [],
}
