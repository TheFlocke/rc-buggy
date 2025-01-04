let _cache =
    { 
        // version muss nur geändert werden, wenn sich die Struktur des Caches ändert,
        // ansonsten werden die aktuellen Dateien immer aus dem Netzwerk geladen!
        // Offline first, network second
        name:'rccar-cache-v1', 
        urls: [
            'index.html',
            'styles.css',
            'main.js',
            'icons-192.png',
            'back.jpg',
            'logo.png',
            'icons-512.png',
            'apple-touch-icon.png'
        ]
    };


// installation of Servicworker -> add all files
self.addEventListener('install', event => {
    console.log(`installing service worker -> adding all files to cache ${_cache.name}`);
    event.waitUntil(
        caches.open(_cache.name)
            .then( cache => {
                    let urls = _cache.urls.map(item => item + (item.indexOf('?') >= 0 ? "&" : "?") + "c=" + _cache.name);
                    return cache.addAll(urls);
                }
            )
    )
});

// Activation of Serviceworker -> cleanup old caches
self.addEventListener('activate', function(event) {
    console.log(`activating service worker -> deleting all old caches: `);
    event.waitUntil(
        caches.keys().then(function(cacheNames) {
            return Promise.all(
                cacheNames.map(function(cacheName) {
                if (cacheName!=_cache.name) {
                    console.log(`    deleting cache ${cacheName}`);
                    return caches.delete(cacheName);
              }
            })
          );
        })
    );
    console.log(`finished activating service worker`);
});

self.addEventListener('fetch', event => {
    console.log(`fetching in service worker`);
    // respond without network
    event.respondWith( 
        fromCache(event.request)
            .then( match => {
                console.log(`cache success for ${event.request.url}`);
                return match;
            })
            .catch(error => {
                console.log(`cache fail for ${event.request.url}`);
                return fetch(event.request);
            }) 
    );
    
    // check for updates
    event.waitUntil(
        // all listed files can be updated
        caches.match(event.request).then( matching=> {
        update(event.request)
            .then(response => { 
                console.log(`replaced cache file for ${event.request.url}`);
                return refresh(response);
          })
          .catch(error => {
                console.log('Fetching failed:', error);
          })
      })
    );
    });

// get file from cache
async function fromCache(request) {
    var url=request.url;
    if(url=="https://apps.abitur-am-berufskolleg.info/rccar/") // wurzel kann auch ohne Dateiname angefragt werden
        url+="index.html";

    url = url + ((url.indexOf('?')>=0 ? "&" : "?") + "c="+_cache.name);

    const matching = await caches.match(url);

    return matching || Promise.reject('no-match');
}

// update file from network
function update(request) {
    return caches.open(_cache.name).then(function (cache) {
        return fetch(request).then(function (response) {
            return cache.put(request, response.clone()).then(function () {
                return response;
            });
        });
    });
}

// force client refresh
function refresh(response) {
    let message = JSON.stringify({
        type: 'refresh',
        url: response.url,
        eTag: response.headers.get('ETag')
    });
    return self.clients.matchAll().then(function (clients) {
        clients.forEach(function (client) {
            client.postMessage(message);
        });
    });
}
