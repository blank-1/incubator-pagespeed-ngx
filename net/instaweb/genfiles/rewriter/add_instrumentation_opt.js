(function(){window.pagespeed=window.pagespeed||{};var f=window.pagespeed;function h(d,a,e,b){this.f=d;this.a=a;this.b=e;this.g=b}f.beaconUrl="";
function k(d){var a=d.f,e=window.mod_pagespeed_start,b=Number(new Date)-e,a=a+(-1==a.indexOf("?")?"?":"&"),a=a+"ets="+("load"==d.a?"load:":"unload:"),a=a+b;if("beforeunload"!=d.a||!window.mod_pagespeed_loaded){a+="&r"+d.a+"=";if(window.performance){var b=window.performance.timing,c=b.navigationStart,g=b.requestStart,a=a+(b[d.a+"EventStart"]-c),a=a+("&nav="+(b.fetchStart-c)),a=a+("&dns="+(b.domainLookupEnd-b.domainLookupStart)),a=a+("&connect="+(b.connectEnd-b.connectStart)),a=a+("&req_start="+(g-
c)),a=a+("&ttfb="+(b.responseStart-g)),a=a+("&dwld="+(b.responseEnd-b.responseStart)),a=a+("&dom_c="+(b.domContentLoadedEventStart-c));window.performance.navigation&&(a+="&nt="+window.performance.navigation.type);c=-1;b.msFirstPaint?c=b.msFirstPaint:window.chrome&&window.chrome.loadTimes&&(c=Math.floor(1E3*window.chrome.loadTimes().firstPaintTime));c=c-g;0<=c&&(a+="&fp="+c)}else a+=b;f.getResourceTimingData&&window.parent==window&&(a+=f.getResourceTimingData());a+=window.parent!=window?"&ifr=1":"&ifr=0";
"load"==d.a&&(window.mod_pagespeed_loaded=!0,(b=window.mod_pagespeed_num_resources_prefetched)&&(a+="&nrp="+b),(b=window.mod_pagespeed_prefetch_start)&&(a+="&htmlAt="+(e-b)));f.criticalCss&&(e=f.criticalCss,a+="&ccis="+e.total_critical_inlined_size+"&cces="+e.total_original_external_size+"&ccos="+e.total_overhead_size+"&ccrl="+e.num_replaced_links+"&ccul="+e.num_unreplaced_links);a+="&dpr="+window.devicePixelRatio;""!=d.b&&(a+=d.b);document.referrer&&(a+="&ref="+encodeURIComponent(document.referrer));
a+="&url="+encodeURIComponent(d.g);f.beaconUrl=a;(new Image).src=a}}f.c=function(d,a,e,b){var c=new h(d,a,e,b);window.addEventListener?window.addEventListener(a,function(){k(c)},!1):window.attachEvent("on"+a,function(){k(c)})};f.addInstrumentationInit=f.c;})();