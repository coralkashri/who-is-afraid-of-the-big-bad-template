/*
Typing Effect
*/

function ClearElementText(element) {
    element.innerHTML = "";
}

function ApplyTyping(element, textToType) {
    ClearElementText(element);
    let i = 0;
    const speed = 30; // milliseconds per character

    function typeWriter() {
        if (i < textToType.length) {
            element.innerHTML += textToType.charAt(i);
            i++;
            setTimeout(typeWriter, speed);
        }
    }

    // Call the function to start the typing effect
    typeWriter();
}

document.addEventListener('DOMContentLoaded', function() {

    /*
    RevealJs Configurations
    */

    Reveal.initialize({
        hash: true,
        transition: 'slide',
        navigationMode: 'grid',
        plugins: [ RevealMarkdown, RevealSearch, RevealHighlight, RevealNotes ]
    });

    /*
    Story PageFlip Configurations
    */

    const pageFlip = new St.PageFlip(
        document.getElementById('flipbook-container'),
        {
            width: 450, // base page width
            height: 450, // base page height

            size: "stretch",
            // set threshold values:
            minWidth: 315,
            maxWidth: 700,
            minHeight: 420,
            maxHeight: 1350,


            maxShadowOpacity: 0.5, // Half shadow intensity
            showCover: true,
            mobileScrollSupport: false // disable content scrolling on mobile devices
        }
    );

    const pages = document.querySelectorAll('.page-slide'); 
    pageFlip.loadFromHTML(pages);

    let isDisclaimerLoaded = false;
    pageFlip.on('flip', (e) => {
        if (e.data == 1 && !isDisclaimerLoaded)
        {
            ApplyTyping(document.getElementById("disclaimer"), document.getElementById("disclaimer-hidden").innerHTML);
            isDisclaimerLoaded = true;
        }
    });
});