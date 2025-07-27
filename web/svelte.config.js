import adapter from '@sveltejs/adapter-static';

const outputFolder = '../server/data/web';

/** @type {import('@sveltejs/kit').Config} */
const config = {
	kit: {
    adapter: adapter({
      pages: outputFolder,
      assets: outputFolder,
      fallback: null
    }),
    vite: {
      build: {
        outDir: outputFolder
      },
      preview: {
        // Serve from /pethome during preview
        outDir: './.svelte-kit/output', // default for preview
        // root: './'
      }
    }
  }
};

export default config;
