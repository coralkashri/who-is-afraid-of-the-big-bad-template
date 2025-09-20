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

    let revealObj = Reveal.initialize({
        hash: true,
        transition: 'slide',
        navigationMode: 'grid',
        plugins: [ RevealMarkdown, RevealSearch, RevealHighlight, RevealNotes ]
    }).then(() => {

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

        let isDisclaimerLoaded = false;
        pageFlip.on('flip', (e) => {
            if (e.data == 1 && !isDisclaimerLoaded)
            {
                ApplyTyping(document.getElementById("disclaimer"), document.getElementById("disclaimer-hidden").innerHTML);
                isDisclaimerLoaded = true;
            }
        });

        /*
        Combined Events
        */

        // helper to test whether we're currently on the book slide
        const BOOK_SLIDE_ID = 'book-slide';
        const isBookSlide = () => Reveal.getCurrentSlide()?.id === BOOK_SLIDE_ID;

        // convenience wrappers for forward/back behaviour
        const flipForward = () => pageFlip.flipNext();
        const flipBackward = () => pageFlip.flipPrev();

        const atFirstPage = () => pageFlip.getCurrentPageIndex() === 0;
        const atLastPage  = () => pageFlip.getCurrentPageIndex() >= pageFlip.getPageCount() - 1;

        let isBookInitialized = false;

        const initializeBook = () => {
            if (isBookSlide()) { // Initialize the book only after revealJs is ready for that and the book is the current slide.
                if (!isBookInitialized) {
                    isBookInitialized = true;
                    const pages = document.querySelectorAll('.page-slide'); 
                    pageFlip.loadFromHTML(pages);
                }
            }
        };

        initializeBook();

        // const showHideNextFregment = (isForward, pageIndex) => {
        //     if (isForward)
        //     {
        //         const fragments = document
        //                             .querySelectorAll('.page-slide')[pageIndex]
        //                             .querySelectorAll('.fragment:not(.visible)');

        //         if (fragments.length > 0) {
        //             fragments[0].classList.add('visible'); // show next fragment
        //             return true; // stop normal navigation
        //         }

        //         return false;
        //     }

        //     let fragments = document
        //                         .querySelectorAll('.page-slide')[pageIndex]
        //                         .querySelectorAll('.fragment.visible');
                        
        //     if (fragments.length > 0) {
        //         fragments[fragments.length - 1].classList.remove('visible'); // hide last fragment
        //         return true;
        //     }

        //     return false
        // };

        const showHideNextFragment = (isForward, pageIndex) => {
            const page = document.querySelectorAll('.page-slide')[pageIndex];
            if (!page) return false;

            const fragments = Array.from(page.querySelectorAll('.fragment'));
            if (fragments.length === 0) return false;

            // find current index
            const currentIdx = fragments.findIndex(f => f.classList.contains('current-visible'));

            if (isForward) {
                if (currentIdx === -1) {
                    // nothing visible yet → show first fragment
                    fragments[0].classList.add('current-visible', 'visible', 'current-fragment');
                    return true;
                }
                if (currentIdx < fragments.length - 1) {
                    // move to next fragment
                    fragments[currentIdx].classList.remove('current-visible', 'visible', 'current-fragment');
                    fragments[currentIdx + 1].classList.add('current-visible', 'visible', 'current-fragment');
                    return true;
                }
                // no more fragments
                return false;
            }

            if (currentIdx > 0) {
                // move back one fragment
                fragments[currentIdx].classList.remove('current-visible', 'visible', 'current-fragment');
                fragments[currentIdx - 1].classList.add('current-visible', 'visible', 'current-fragment');
                return true;
            }
            if (currentIdx === 0) {
                return false;
                // hide the very first fragment
                fragments[0].classList.remove('current-visible');
                return true;
            }
            // nothing was visible at all
            return false;
        };

        // register keys that should behave as "forward" (Right, Down, PageDown, Space)
        const forwardKeys = [39, 40, 34, 32]; // keyCodes: ArrowRight, ArrowDown, PageDown, Space
        forwardKeys.forEach(code => {
            Reveal.addKeyBinding(code, () => {
                if (!isBookSlide()) {
                    // not on the book slide → do normal Reveal navigation
                    Reveal.next();
                    initializeBook(); // Initialize the book only after revealJs is ready for that and the book is the current slide.
                    return;
                }

                // on book slide → try to flip inside the book
                if (!atLastPage()) {
                    const visiblePage = pageFlip.getCurrentPageIndex();
                    
                    if (showHideNextFragment(true, visiblePage)) return;
                    if (showHideNextFragment(true, visiblePage + 1)) return;

                    if (visiblePage < pageFlip.getPageCount() - 1) {
                        pageFlip.flipNext();
                        return;
                    }
                } else {
                    // last page → move to the next Reveal slide after the book
                    Reveal.next();
                }
            });
        });

        // register keys that should behave as "back" (Left, Up, PageUp)
        const backKeys = [37, 38, 33]; // ArrowLeft, ArrowUp, PageUp
        backKeys.forEach(code => {
            Reveal.addKeyBinding(code, () => {
                if (!isBookSlide()) {
                    Reveal.prev();
                    initializeBook(); // Initialize the book only after revealJs is ready for that and the book is the current slide.
                    return;
                }

                if (!atFirstPage()) {
                    const visiblePage = pageFlip.getCurrentPageIndex();
                    if (!atLastPage()) {
                        if (showHideNextFragment(false, visiblePage + 1)) return;
                        if (showHideNextFragment(false, visiblePage)) return;
                    }

                    if (visiblePage > 0) {
                        pageFlip.flipPrev();
                        return;
                    }
                } else {
                    // first page → go to previous Reveal slide before the book
                    Reveal.prev();
                }
            });
        });

        // Optional: clean-up function if you ever want to remove these custom bindings
        const removeBindings = () => {
            forwardKeys.concat(backKeys).forEach(k => Reveal.removeKeyBinding(k));
        };

        // done — now arrows/page keys are integrated with the book
    });
});