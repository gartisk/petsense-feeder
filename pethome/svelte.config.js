import adapter from '@sveltejs/adapter-static';

/** @type {import('@sveltejs/kit').Config} */
const config = {
	kit: {
    adapter: adapter({
      pages: '../data',     // folder where the HTML will go
      assets: '../data',    // folder where assets will go
      fallback: null
    }),
  }
};

export default config;
