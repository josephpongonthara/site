document.addEventListener("DOMContentLoaded", () => {
  // CODE EDITOR
  document.querySelectorAll(".code-editor").forEach((editor, index) => {
    const tabs = editor.querySelectorAll(".tab");
    const codeBlock = editor.querySelector(".code-block");

    if (!codeBlock) {
      console.warn(`No code block found in editor #${index}`);
      return;
    }

    const loadCode = async (tab) => {
      tabs.forEach(t => t.classList.remove("active"));
      tab.classList.add("active");
      const file = tab.dataset.file;
      const language = tab.dataset.lang.toLowerCase();
      try {
        const response = await fetch(file);
        const code = await response.text();
        codeBlock.textContent = code;
        codeBlock.className = `language-${language} code-block`;
        Prism.highlightElement(codeBlock);
        codeBlock.dataset.loaded = "true";
      } catch (error) {
        codeBlock.textContent = "Error loading file.";
        console.error(`Failed to load ${file}:`, error);
      }
    };

    tabs.forEach((tab) => {
      tab.addEventListener("click", () => loadCode(tab));
    });

    const rect = editor.getBoundingClientRect();
    const isVisible = rect.top < window.innerHeight && rect.bottom > 0;

    if (tabs.length > 0 && isVisible) {
      loadCode(tabs[0]);
    }
    });

    // FADE-IN + visibility-on-load fix
    const faders = document.querySelectorAll(".fade-in");
    const appearOptions = {
        threshold: 0.2,
        rootMargin: "0px 0px -50px 0px",
    };

    const appearOnScroll = new IntersectionObserver((entries, observer) => {
        entries.forEach((entry) => {
        if (!entry.isIntersecting) return;
        entry.target.classList.add("visible");
        observer.unobserve(entry.target);
        });
    }, appearOptions);

    faders.forEach((fader) => {
        appearOnScroll.observe(fader);

        // Force visibility if already in view (DOM-level)
        if (fader.getBoundingClientRect().top < window.innerHeight) {
        fader.classList.add("visible");
        }
    });

    // Force visibility if already in view (after full layout)
    window.addEventListener("load", () => {
        document.querySelectorAll(".fade-in").forEach((el) => {
        const rect = el.getBoundingClientRect();
        const isVisible = rect.top < window.innerHeight && rect.bottom > 0;
        if (isVisible) {
            el.classList.add("visible");
        }
        });
    });

    // IMAGE CAROUSEL
    const images = [
        { src: "assets/images/bikebuddy/arduino_enclosure_bottom.png", caption: "Arduino Enclosure Bottom"},
        { src: "assets/images/bikebuddy/arduino_enclosure_top.png", caption: "Arduino Enclosure Top"},
        { src: "assets/images/bikebuddy/screenmount_solidworks.png", caption: "Screen Mount (SolidWorks)"},
        { src: "assets/images/bikebuddy/handlebarclamp_bottom.png", caption: "Handlebar Clamp (Bottom)"},
        { src: "assets/images/bikebuddy/handlebarclamp_top.png", caption: "Handlebar Clamp (Top)"},
        { src: "assets/images/bikebuddy/batterymount_top_isometric.png", caption: "Battery Mount (Top, Isometric)"},
        { src: "assets/images/bikebuddy/batterymount_top_1.png", caption: "Battery Mount (Top View)"},
        { src: "assets/images/bikebuddy/batterymount_bottom.png", caption: "Battery Mount (Bottom View)"},
        { src: "assets/images/bikebuddy/seatpost_mount.png", caption: "Seatpost Mount"},
        { src: "assets/images/bikebuddy/sensor_mount_isometric.png", caption: "Sensor Mount (Isometric)"},
        { src: "assets/images/bikebuddy/sensor_mount_top.png", caption: "Sensor Mount (Top View)"},
        { src: "assets/images/bikebuddy/sensor_mount_lid.png", caption: "Sensor Mount Lid"}
    ];

    let index = 0;
    function showImage(i) {
        const imageEl = document.getElementById("carousel-image");
        const captionEl = document.getElementById("carousel-caption");

        imageEl.src = images[i].src;
        captionEl.innerHTML = `<em>${images[i].caption}</em>`;
        }

    function prevImage() {
        index = (index - 1 + images.length) % images.length;
        showImage(index);
        }

    function nextImage() {
        index = (index + 1) % images.length;
        showImage(index);
        }

    window.prevImage = prevImage;
    window.nextImage = nextImage;
    showImage(index);
    
    //ball & beam
    // Toggle Button Function
    function toggleSection(id) {
        const section = document.getElementById(id);
        const isNowHidden = section.classList.toggle("hidden");
        if (!isNowHidden) {
        section.classList.add("visible"); // Trigger fade-in manually when shown
        }
        const button = document.querySelector(`button[onclick="toggleSection('${id}')"]`);
        if (section.classList.contains("hidden")) {
            button.textContent = "See Details";
        } else {
            button.textContent = "Hide Details";
        }
    }
    window.toggleSection = toggleSection;
    // Ball & Beam Image Carousel
    const ballbeamImages = [
        { src: "assets/images/ballbeam/system.png", caption: "Complete System"},
        { src: "assets/images/ballbeam/simulink.png", caption: "Complete Simulink Model System"},
        { src: "assets/images/ballbeam/input.png", caption: "MATLAB Simulation meeting Input Voltage Saturation Requirement"},
        { src: "assets/images/ballbeam/output_c1.png", caption: "MATLAB Simulation meeting Overshoot Requirement for Gear Angle Controller"},
        { src: "assets/images/ballbeam/output_c2.png", caption: "MATLAB Simulation meeting Overshoot Requirement for Ball Position Controller"}
    ];
    let ballbeamIndex = 0;
    function showBallBeamImage(i) {
        const image = document.getElementById("ballbeam-carousel-image");
        const caption = document.getElementById("ballbeam-caption");
        image.src = ballbeamImages[i].src;
        caption.textContent = ballbeamImages[i].caption;
    }
    function prevBallBeamImage() {
        ballbeamIndex = (ballbeamIndex - 1 + ballbeamImages.length) % ballbeamImages.length;
        showBallBeamImage(ballbeamIndex);
    }
    function nextBallBeamImage() {
        ballbeamIndex = (ballbeamIndex + 1) % ballbeamImages.length;
        showBallBeamImage(ballbeamIndex);
    }
    window.prevBallBeamImage = prevBallBeamImage;
    window.nextBallBeamImage = nextBallBeamImage;
    showBallBeamImage(ballbeamIndex);

    //more projects section
    const sarImages = [
        'assets/images/search/model.png',
        'assets/images/search/print.png',
        'assets/images/search/complete.png'
    ];

    let currentSARImage = 0;

    function updateSARImage() {
        const img = document.getElementById('sar-carousel-image');
        img.src = sarImages[currentSARImage];
    }

    function prevSARImage() {
        currentSARImage = (currentSARImage - 1 + sarImages.length) % sarImages.length;
        updateSARImage();
    }

    function nextSARImage() {
        currentSARImage = (currentSARImage + 1) % sarImages.length;
        updateSARImage();
    }
    window.prevSARImage = prevSARImage;
    window.nextSARImage = nextSARImage;
    updateSARImage(currentSARImage);
    // Load external Python code into mini viewer
    fetch('assets/code/ball/basketball.py')
    .then(response => response.text())
    .then(code => {
        document.getElementById('draft-code').textContent = code;
        if (window.Prism) Prism.highlightElement(document.getElementById('draft-code'));
    });

});
