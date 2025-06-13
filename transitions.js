// Direction-based page transition using sessionStorage
window.addEventListener('DOMContentLoaded', () => {
  const body = document.body;
  const pageOrder = ['home', 'about', 'projects', 'personal'];
  
  // Extract base class name (e.g., 'about-page' -> 'about')
  const currentPage = Array.from(body.classList)
    .find(cls => cls.endsWith('-page'))
    ?.replace('-page', '') || 'home';

  const prevPage = sessionStorage.getItem('lastPage');
  if (prevPage && pageOrder.includes(prevPage) && pageOrder.includes(currentPage)) {
    const fromIndex = pageOrder.indexOf(prevPage);
    const toIndex = pageOrder.indexOf(currentPage);
    const direction = toIndex > fromIndex ? 'down' : 'up';

    body.classList.add(`page-enter-${direction}`);
    requestAnimationFrame(() => {
      body.classList.add('page-enter-active');
    });
  }

  sessionStorage.setItem('lastPage', currentPage);
});

// Swup fade transition setup
const swup = new Swup({
  containers: ['#swup'],
  animations: [
    {
      name: 'fade',
      from: '(.*)',
      to: '(.*)',
      out: (next) => {
        document.getElementById('swup').classList.add('fade-out');
        setTimeout(next, 300);
      },
      in: (next) => {
        document.getElementById('swup').classList.remove('fade-out');
        next();
      },
    },
  ],
});

// Optional: Reinitialize your dynamic JS logic after Swup page load
swup.on('animationInDone', () => {
  if (document.body.classList.contains('about-page')) {
    if (typeof initAboutTypewriter === 'function') initAboutTypewriter();
  } else if (document.body.classList.contains('home-page')) {
    if (typeof initIntroTypewriter === 'function') initIntroTypewriter();
  }
});

requestAnimationFrame(() => {
  body.classList.add('page-enter-active');
});