document.addEventListener("DOMContentLoaded", function () {
  const aboutText = "Recent engineering graduate exploring solutions through hands-on projects and diverse technical tools.";
  const aboutTypedText = document.getElementById("about-text");

  if (aboutTypedText) {
    let index = 0;
    function type() {
      if (index < aboutText.length) {
        aboutTypedText.textContent += aboutText.charAt(index);
        index++;
        setTimeout(type, 50);
      } else {
        // Fade-in all other elements with class .fade-in
        document.querySelectorAll(".fade-in").forEach((el, i) => {
          setTimeout(() => {
            el.classList.add("visible");
          }, i * 100);

          // Force visibility if already in view
          if (el.getBoundingClientRect().top < window.innerHeight) {
            el.classList.add("visible");
          }
        });
      }
    }
    type();
  }
});
