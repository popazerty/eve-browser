
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

void addLinkHighlighting(WebKitWebView* web_view) {

    // Well this is here a mess cause I tried different approaches to enable link higlighting
    // Each one has flaws up to performace drain.

    /*
    char scriptBackgroundColor[] = "document.styleSheets[0].insertRule(\'a{backgroundColor=\"#FF0000\";}\',0);";
    */

    /*
    char scriptBackgroundColor[] = "var cssTags = document.getElementsByTagName(\"style\"); \
        var cssTag = cssTags[0]; \
        alert(cssTag.innerText); \
        cssTag.innerText += \'a { backgroundColor = \"#FF0000\"; }\'; \
        alert(cssTag.innerText);";
    */

    /*
    char scriptBackgroundColor[] = "var cssTag = document.createElement(\"style\"); \
        cssTag.type = \"text/css\"; \
        cssTag.innerHtml = \'a { backgroundColor = \"#FF0000\"; }\'; \
        document.getElementsByTagName(\"head\")[0].appendChild(cssTag);";
    */
  
          
    // cssTag.getElementsByTagName.innerHtml = \'a { backgroundColor = \"#FF0000\" }\';

    /*
    char scriptBackgroundColor[] = "var a = document.getElementsByTagName(\"a\"); \
    for(var i=0;i<a.length;i++){ \
        a[i].style.backgroundColor = \"#FFA4A4\"; \
        a[i].style.border = \"#FF0000 solid medium\"; \
        a[i].style.borderRadius = \"15px\"; \
    }";
    */

    //webkit_web_view_execute_script(web_view, scriptBackgroundColor);

    //--------------------------

    /*
    char scriptHover[] = "var cssTag = document.createElement(\"style\"); \
        cssTag.setAttribute(\"type\",\"text/css\"); \
        cssTag.innerHtml = \'a:focus { backgroundColor = \"#00FF00\" }\'; \
        document.body.appendChild(cssTag);";
    */

    /*
    char scriptHover[] = "var a = document.getElementsByTagName(\"*\"); \
        for(var i=0;i<a.length;i++) { \
            a[i].onfocus = function() { \
                this.style.backgroundColor= \"#87FF87\"; \
                this.style.border = \"#00FF00 solid medium\"; \
                this.style.borderRadius = \"15px\"; \
             }; \
			 a[i].onblur = function() { \
                this.style.backgroundColor= \"#FFA4A4\"; \
                this.style.border = \"#FF0000 solid medium\"; \
                this.style.borderRadius = \"15px\"; \
            }; \
        }";
    */
    
    //webkit_web_view_execute_script(web_view, scriptHover);
}

void registerCssExtension(WebKitWebView* web_view)
{
    addLinkHighlighting(web_view);
}

