document.addEventListener("DOMContentLoaded", function () {
  // Fade in all elements with class .fade-in
  document.querySelectorAll(".fade-in").forEach((el, i) => {
    setTimeout(() => {
      el.classList.add("visible");
    }, i * 100); // Staggered fade-in effect
  });
});
