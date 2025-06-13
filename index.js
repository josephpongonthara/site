// Typewriter for Intro page
document.addEventListener("DOMContentLoaded", function () {
  const text = "Joseph Pongonthara";
  const typedText = document.getElementById("typed-text");
  const navLinks = document.querySelector(".nav-links");

  if (typedText) {
    let index = 0;
    function type() {
      if (index < text.length) {
        typedText.textContent += text.charAt(index);
        index++;
        setTimeout(type, 200);
      } else {
        setTimeout(() => {
          navLinks.classList.add("fade-in");
          // Force visibility if already in view
          if (navLinks.getBoundingClientRect().top < window.innerHeight) {
            navLinks.classList.add("visible");
          }
        }, 300);
      }
    }
    type();
  }
});
