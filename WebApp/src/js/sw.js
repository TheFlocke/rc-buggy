let _cache =
    {
        name:`rcrover-cache-${version}`,
        urls: [
            '/index.html',
            '/src/styles.css',
            '/src/js/main.js',
            '/src/js/window.js',
            '/src/js/debug.js',
            '/img/arm_0.png',
            '/img/arm_1.png',
            '/img/arm_2.png',
            '/img/logo.png',
            '/img/logo-inverted.png',
            '/icons/apple-touch-icon.png',
            '/icons/favicon.ico',
            '/icons/favicon.svg',
            '/icons/favicon-96x96.png',
            '/icons/site.webmanifest',
            '/icons/web-app-manifest-192x192.png',
            '/icons/web-app-manifest-512x512.png'
        ]
    };


self.addEventListener('install', event => {
    event.waitUntil(
        caches.open(_cache.name)
            .then(cache => cache.addAll(_cache.urls))
    );
});

self.addEventListener('activate', event => {
    event.waitUntil(
        caches.keys().then(cacheNames =>
            Promise.all(
                cacheNames.map(cacheName => {
                    if (cacheName !== _cache.name) {
                        return caches.delete(cacheName);
                    }
                })
            )
        )
    );
});

self.addEventListener('fetch', event => {
    event.respondWith(
        caches.match(event.request)
            .then(response => response || fetch(event.request))
            .catch(() => caches.match('/index.html')) // fallback for navigation
    );
});
