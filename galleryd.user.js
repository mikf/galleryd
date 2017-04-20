// ==UserScript==
// @name        galleryd
// @namespace   util
// @include     https://www.pixiv.net/*
// @include     https://exhentai.org/*
// @include     https://saucenao.com/search.php*
// @include     https://gelbooru.com/*
// @include     https://chan.sankakucomplex.com/*
// @include     https://nijie.info/*
// @grant       GM_xmlhttpRequest
// ==/UserScript==

"use strict";

////////////////////////////////////////////////////////////////////////////////
// Constants

let STATE_MISSING = 0;
let STATE_PENDING = 1;
let STATE_DONE    = 2;


////////////////////////////////////////////////////////////////////////////////
// Modules

/*
 * Module Interface
 * ----------------
 *
 * function module_name()
 * {
 *   this.category --- category-name, that all item emitted by this module
 *                     will be associated with (optional)
 *
 *   this.run      ---
 *
 *   this.show     ---
 *
 * }
 */

// Pixiv
function pixiv_home()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        callback(this, items, nodes);
    }

    this.show = function(nodes, states)
    {
        for(let i = 0; i < nodes.length; ++i)
        {
            //TODO
        }
    }
}

function pixiv_bookmarks()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let bookmarks = document.getElementsByClassName("ui-profile-popup");

        for(let i = 0; i < bookmarks.length; ++i)
        {
            items.push(bookmarks[i].getAttribute("data-user_id"));
            nodes.push(bookmarks[i].nextSibling.nextSibling);
        }

        util_add_style(".count-list .gd-info {background-color:#BFB; font-size:10pt !important; vertical-align:bottom}\
                   .image-item .gd-info {margin-left:5px; font-size: 10pt}\
                   .image-item .title {max-height: 36px; overflow:hidden}" );
        callback(this, items, nodes);

        let target = document.getElementById("illust-recommend").firstChild.nextSibling;
        util_observe(this, target, callback);
    }

    this.observe = function(callback, n)
    {
        let items = [];
        let nodes = [];

        for(let j = 0; j < n.length; ++j)
        {
            items.push(n[j].lastChild.getAttribute("data-user_id"));
            nodes.push(n[j]);
        }

        callback(this, items, nodes);
    }

    this.show = function(nodes, states)
    {
        if(nodes[0].tagName == "UL")
        {
            for(let i = 0; i < nodes.length; ++i)
            {
                let l = document.createElement("li");
                let s = states[i];

                s.className += " bookmark-count";
                l.appendChild(s);
                nodes[i].appendChild(l);
            }
        }
        else
        {
            util_append(nodes, states);
        }
    }
}

function pixiv_bookmark_add()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        util_add_style(".image-item .gd-info {margin-left:5px; font-size: 10pt}\
                   .image-item .title {max-height: 36px; overflow:hidden}" );

        let target = document.getElementById("illust-recommend").firstChild.nextSibling;
        util_observe(this, target, callback);
    }

    this.observe = function(callback, n)
    {
        let items = [];
        let nodes = [];

        for(let j = 0; j < n.length; ++j)
        {
            items.push(n[j].lastChild.getAttribute("data-user_id"));
            nodes.push(n[j]);
        }

        callback(this, items, nodes);
    }

    this.show = util_append;
}

function pixiv_member()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let url  = document.getElementsByClassName("user-link")[0].getAttribute("href");
        let tabs = document.getElementsByClassName("tabs")[0];
        let item = url.substring(15); // "/member.php?id=".length

        util_add_style(".tabs > li > .gd-info{float:right; padding-right:8px; font-size:16pt; line-height:24px}");
        callback(this, [item], [tabs]);
    }

    this.show = function(nodes, states)
    {
        let l = document.createElement("li");
        let s = states[0];

        l.appendChild(s);
        nodes[0].appendChild(l);
    };
};

function pixiv_member_bookmarks()
{
    this.parent = new pixiv_member();

    this.category = "pixiv";

    this.run = function(callback)
    {
        let items, nodes;
        let bookmarks = document.getElementsByClassName("ui-profile-popup");

        this.parent.run(function(p, i, n)
        {
            items = i;
            nodes = n;
        });

        for(let i = 0; i < bookmarks.length; ++i)
        {
            items.push(bookmarks[i].getAttribute("data-user_id"));
            nodes.push(bookmarks[i].parentNode.lastChild);
        }

        util_add_style(".count-list .gd-info { background-color: #BFB; }");
        callback(this, items, nodes);
    }

    this.show = function(nodes, states)
    {
        this.parent.show(nodes, states);

        for(let i = 1; i < nodes.length; ++i)
        {
            let l = document.createElement("li");
            let s = states[i];

            s.className += " bookmark-count";
            l.appendChild(s);
            nodes[i].appendChild(l);
        }
    }
};

function pixiv_search()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let users = document.getElementsByClassName("user");

        for(let i = 0; i < users.length; ++i)
        {
            items.push(users[i].getAttribute("data-user_id"));
            nodes.push(users[i].parentNode);
        }

        util_add_style(".count-list .gd-info {background-color:#BFB; font-size:10pt !important; vertical-align:bottom}\
                   .image-item .title {max-height: 36px; overflow:hidden}" );
        callback(this, items, nodes);
    };

    this.show = function(nodes, states)
    {
        for(let i = 0; i < nodes.length; ++i)
        {
            let l = document.createElement("li");
            let p = nodes[i];
            let s = states[i];

            s.className += " bookmark-count";
            l.appendChild(s);

            if(p.lastChild.tagName == "UL")
            {
                p.lastChild.appendChild(l);
            }
            else
            {
                let u = document.createElement("ul");
                u.appendChild(l);
                p.appendChild(u);
            }
        }
    }
};

function pixiv_stacc()
{
    this.category = "pixiv";

    this.run = function(callback)
    {

    }

    this.items = function()
    {
        let items = [];
        let users = document.getElementsByClassName("stacc_ref_illust_user_name_caption");

        for(let i = 0; i < users.length; ++i)
        {
            let href = users[i].firstChild.getAttribute("href");
            let end  = href.indexOf("&");
            items.push(href.substring(15, end)); // "/member.php?id=".length
        }

        return items;
    }

    this.display = function(items, state)
    {
        let users = document.getElementsByClassName("stacc_ref_illust_user_name_caption");

        for(let i = 0; i < users.length; ++i)
        {
            users[i].innerHTML +=
                "<span style='" + util_get_indicator(state[i]) + "</span>";
        }
    }
}

function pixiv_spotlight()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let items  = [];
        let nodes  = [];
        let users = document.getElementsByClassName("ui-profile-popup");

        for(let i = 0; i < users.length; ++i)
        {
            items.push(users[i].getAttribute("data-user_id"));
            nodes.push(users[i].parentNode);
        }

        util_add_style(".gd-info { margin-left: 5px }");
        callback(this, items, nodes);
    };

    this.show = util_append;
};

function pixiv_ranking()
{
    this.category = "pixiv";

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let users = document.getElementsByClassName("ui-profile-popup");

        for(let i = 0; i < users.length; ++i)
        {
            items.push(users[i].getAttribute("data-user_id"));
            nodes.push(users[i].parentNode.firstChild);
        }

        util_add_style(".gd-info {margin-left:5px; vertical-align:1px; font-size:14px}");
        callback(this, items, nodes);

        let target = document.getElementById("1").parentNode;
        util_observe(this, target, callback);
    }

    this.observe = function(callback, n)
    {
        let items = [];
        let nodes = [];

        for(let j = 0; j < n.length; ++j)
        {
            items.push(n[j].lastChild.getAttribute("data-user_id"));
            nodes.push(n[j].firstChild);
        }

        callback(this, items, nodes);
    }

    this.show = function(nodes, states)
    {
        for(let i = 0; i < nodes.length; ++i)
            nodes[i].replaceChild(states[i], nodes[i].lastChild);
    }
};

// Exhentai
function exhentai_gallery()
{
    this.category = "exhentai";

    this.run = function(callback)
    {
        let url = window.location.href;
        let end = url.lastIndexOf("/");
        let item = url.substring(22, end); // "http://exhentai.org/g/".length
        let node = document.getElementById("gd2");

        util_add_style(".gd-info {float:right; font-size: 16pt}");
        callback(this, [item], [node]);
    }

    this.show = function(nodes, states)
    {
        nodes[0].insertBefore(states[0], nodes[0].firstChild);
    }
};

function exhentai_image()
{
    this.category = "exhentai";

    this.run = function(callback)
    {
        let url = document.getElementById("i5")
            .firstChild.firstChild
            .getAttribute("href");
        let item = url.substring(22, url.length-1); // "http://exhentai.org/g/".length
        let node = document.getElementById("i1");

        util_add_style(".gd-info {float:right; padding-top:4px; font-size: 16pt}");
        callback(this, [item], [node]); // TODO
    }

    this.show = function(nodes, states)
    {
        nodes[0].insertBefore(states[0], nodes[0].firstChild);
    }
};

function exhentai_search()
{
    this.category = "exhentai";

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let links = document.getElementsByClassName("it5");

        for(let i = 0; i < links.length; ++i)
        {
            let link = links[i].firstChild.getAttribute("href");
            items.push(link.substring(22, link.length-1));
            nodes.push(links[i].previousSibling);
        }

        util_add_style(".gd-info {float:left}");
        callback(this, items, nodes);
    };

    this.show = function(nodes, states)
    {
        for(let i = 0; i < nodes.length; ++i)
        {
            if(states[i].className.indexOf("gd-missing") != -1)
                continue;

            nodes[i].insertBefore(states[i], nodes[i].firstChild);
        }
    };
};

// SauceNao
function saucenao_search()
{
    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let pattern = /http:\/\/(www\.pixiv\.net|nijie\.info)\/members?\.php\?id=\d+/;
        let links   = document.getElementsByClassName("linkify");

        for(let i = 0; i < links.length; ++i)
        {
            let href = links[i].getAttribute("href");
            if(pattern.test(href))
            {
                items.push(href);
                nodes.push(links[i]);
            }
        }

        util_add_style(".gd-info {margin-left:10px; font-size:14pt;}");
        callback(this, items, nodes);
    }

    this.show = function(nodes, states)
    {
        for(let i = 0; i < nodes.length; ++i)
            nodes[i].parentNode.replaceChild(states[i], nodes[i].nextSibling);
    }
}


// Gelbooru
function gelbooru_search()
{
    this.category = "gelbooru";
    this.offset   = 32; // "index.php?page=post&s=list&tags=".length

    this.run = function(callback)
    {
        let items = [];
        let nodes = [];
        let artists = document.getElementById("tag-sidebar")
                              .getElementsByClassName("tag-type-artist");

        for(let i = 0; i < artists.length; ++i)
        {
            let href = artists[i].lastElementChild.previousElementSibling.getAttribute("href");
            items.push(unescape(href.substring(this.offset)));
            nodes.push(artists[i]);
        }

        util_add_style(".gd-info {margin-left:3px;}");
        callback(this, items, nodes);
    }

    this.show = util_append;
}

// Sankaku
function sankaku_search()
{
    this.category = "sankaku";
    this.offset   = 7; // "/?tags=".length
}
sankaku_search.prototype = new gelbooru_search();

// Nijie
function nijie_member()
{
    this.category = "nijie";

    this.run = function(callback)
    {
        let url = document.getElementsByClassName("name")[0].getAttribute("href");
        let item = url.substring(15); // "members.php?id="
        let node = document.getElementById("members_left");

        util_add_style(".gd-info{float:right; padding:12px 5px 0 0; font-size:16pt}");
        callback(this, [item], [node]);
    }

    this.show = function(nodes, states)
    {
        nodes[0].insertBefore(states[0], nodes[0].firstChild);
    }
}

function nijie_member_works()
{
    this.category = "nijie";

    this.run = function(callback)
    {
        let url = document.getElementsByClassName("name")[0].getAttribute("href");
        let item = url.substring(15); // "members.php?id="
        let node = document.getElementById("content-upload-menu");

        util_add_style(".gd-info{float:right; padding:12px 5px 0 0; font-size:16pt}");
        callback(this, [item], [node]);
    }

    this.show = function(nodes, states)
    {
        let n = nodes[0];
        n.parentNode.insertBefore(states[0], n.nextSibling);
    }
}

//
function select_module()
{
    let host = window.location.hostname;
    let path = window.location.pathname;

    if(host == "www.pixiv.net")
    {
        if(path.startsWith("/mypage.php"))
            return new pixiv_home();

        if(path.startsWith("/member"))
            return new pixiv_member();

        if(path.startsWith("/bookmark.php?id="))
            return new pixiv_member_bookmarks();

        if(path.startsWith("/bookmark.php"))
            return new pixiv_bookmarks();

        if(path.startsWith("/bookmark_add.php"))
            return new pixiv_bookmark_add();

        if(path.startsWith("/search.php"))
            return new pixiv_search();

        if(path.startsWith("/stacc/"))
            return new pixiv_stacc();

        if(path.startsWith("/spotlight/"))
            return new pixiv_spotlight();

        if(path.startsWith("/ranking.php"))
            return new pixiv_ranking();
    }

    else if(host == "exhentai.org")
    {
        if(path.startsWith("/g/"))
            return new exhentai_gallery();

        if(path.startsWith("/s/"))
            return new exhentai_image();

        return new exhentai_search();
    }

    else if(host == "saucenao.com")
    {
        return new saucenao_search();
    }

    else if(host == "gelbooru.com")
    {
        return new gelbooru_search();
    }

    else if(host == "chan.sankakucomplex.com")
    {
        return new sankaku_search();
    }

    else if(host == "nijie.info")
    {
        if(path.startsWith("/members.php"))
            return new nijie_member();

        if(path.startsWith("/members_illust.php"))
            return new nijie_member_works();
    }
}


////////////////////////////////////////////////////////////////////////////////
// Requests

function request_status(module, items, nodes)
{
    if(!items || items.length == 0) return;

    let request = {
        "method"  : "status",
        "items"   : items
    };

    if(module.category)
        request.category = module.category;

    GM_xmlhttpRequest({
        method: "POST",
        url   : "http://localhost:6411/",
        data  : JSON.stringify(request),
        onload: function(response)
        {
            let json = JSON.parse(response.responseText);
            if(json.error)
            {
                console.log(json.error);
                return;
            }

            let s = json.state;
            for(let i = 0; i < s.length; ++i)
                s[i] = util_get_indicator(module, items[i], s[i]);
            module.show(nodes, s);
        }
    });
}

function request_add(module, item)
{
    if(!item || item.trim() == "") return;

    let node = this;

    let request = {
        "method"  : "add",
        "items"   : item
    };

    if(module.category)
        request.category = module.category;

    GM_xmlhttpRequest({
        method: "POST",
        url   : "http://localhost:6411/",
        data  : JSON.stringify(request),
        onload: function(response)
        {
            let json = JSON.parse(response.responseText);
            if(json.error)
            {
                console.log(json.error);
                return;
            }

            let si = util_get_indicator(module, item, STATE_PENDING);
            node.parentNode.replaceChild(si, node);
        }
    });
}

function request_download(module, item)
{

}

function request_open(module, item)
{
    if(!item) return;

    let request = {
        "method"  : "open",
        "items"   : item
    };

    if(module.category)
        request.category = module.category;

    GM_xmlhttpRequest({
        method: "POST",
        url   : "http://localhost:6411/",
        data  : JSON.stringify(request),
        // onload: function(response)
        // {
        // }
    });
}

////////////////////////////////////////////////////////////////////////////////
// Utility

function util_get_indicator(module, item, state)
{
    let obj = document.createElement("a");

    switch(state)
    {
    case STATE_MISSING:
        obj.className = "gd-info gd-missing";
        obj.textContent = "❌";
        obj.addEventListener("dblclick", request_add.bind(obj, module, item), true);
        break;

    case STATE_PENDING:
        obj.className = "gd-info gd-pending";
        obj.textContent = "◉";
        obj.addEventListener("dblclick", request_download.bind(obj, module, item), true);
        break;

    case STATE_DONE:
        obj.className = "gd-info gd-done";
        obj.textContent = "✔";
        obj.addEventListener("dblclick", request_open.bind(obj, module, item), true);
        break;

    default:
        obj.textContent = "?";
        break;
    }

    return obj;
}

function util_append(nodes, states)
{
    for(let i = 0; i < nodes.length; ++i)
        nodes[i].appendChild(states[i]);
}

function util_observe(module, target, callback)
{
    let mo = new MutationObserver(util_observe_handler.bind(module, callback));
    let mc = { attributes: true, childList: true, characterData: true };
    mo.observe(target, mc);
}

function util_observe_handler(callback, mutation)
{
    for(let i = 0; i < mutation.length; ++i)
    {
        if(mutation[i].type == "childList")
            this.observe(callback, mutation[i].addedNodes);
    }
}

function util_add_style(css)
{
    let style = document.createElement("style");
    style.setAttribute("type", "text/css");
    style.textContent = css;
    document.head.appendChild(style);
}


////////////////////////////////////////////////////////////////////////////////
// Main

let module = select_module();
if(module)
{
    util_add_style("\
.gd-info    { cursor: pointer; }\
.gd-missing { color:#F00 !important; }\
.gd-pending { color:#66F !important; margin-top: -2px }\
.gd-done    { color:#0C0 !important; }\
    ");
    module.run(request_status);
}
