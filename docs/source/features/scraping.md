Scraping Websites
=================
```{warning}
Only proceed if you consider yourself a power user, and you know what you are doing!
```

RSS Guard offers additional advanced feature inspired by [Liferea](https://lzone.de/liferea).

Goal of this feature is to allow advanced users to use RSS Guard with data sources which do not provide regular feed. So you can use the feature to generate one.

----
You can select type of source of each feed. Currently these sources are supported:
* URL - RSS Guard simply downloads feed file from given location and behaves like everyone would expect.
* Local file - RSS Guard uses file in local filesystem as feed source.
* Script - see below.
* Built-in web browser with JavaScript support - see below.

## 'Script' option
If you choose `Script` option, then you cannot provide URL of your feed, and you rely on custom script to generate feed file and provide its contents to [**standard output** (stdout)](https://en.wikipedia.org/wiki/Standard_streams#Standard_output_(stdout)). Data written to standard output should be valid feed data.

`Fetch it now` button also works with `Script` option. Therefore, if your source script and (optional) post-process script in cooperation deliver a valid feed data to the output, then all important metadata, like title or icon of the feed, can be discovered :sparkles: automagically :sparkles:.

<img alt="alt-img" src="images/scrape-source-type.png" width="350px">

Any errors in your script must be written to [**error output** (stderr)](https://en.wikipedia.org/wiki/Standard_streams#Standard_error_(stderr)).

:::{warning}
If your path to executable contains backslashes as directory separators, make sure to escape them with another backslash. Quote each individual argument with double quotes `"arg"` or single quotes `'arg'` and separate all arguments with spaces. You have to escape some characters inside double-quoted argument, for example double quote itself like this `"arg with \"quoted\" part"`.

Examples (one per line):

```
C:\\MyFolder\\My.exe "arg1" "arg2" "my \"quoted\" arg3" 'my "quoted" arg4'

bash "%data%/scripts/download-feed.sh"

%data%\jq.exe '{ version: "1.1", title: "Stars", items: map( . | .title=.full_name | .content_text=.description | .date_published=.pushed_at)}'
```
:::

RSS Guard offers [placeholder](userdata.md#data-placeholder) `%data%` which is automatically replaced with full path to RSS Guard user data folder and you can use this placeholder anywhere in your script call line.

```{attention}
Working directory of process executing the script is set to point to RSS Guard [user data](userdata) folder.
```

Format of post-process script execution line can be seen on picture below.

<img alt="alt-img" src="images/scrape-post.png" width="350px">

If everything goes well, script must return `0` as the process exit code, or a non-zero exit code if some error happened.

Executable file must be always be specified, while arguments do not. Be very careful when quoting arguments. Tested examples of valid execution lines are:

## 'Built-in web browser with JavaScript support' option
This option uses network stack from built-in web browser (based on Chromium) to download the provided file. There are some benefits to this approach:
* all cookies and other cache/storage data from internal web browser persist,
* JavaScript-enabled websites can be loaded.

So, if you for example login to some of your websites, then you can use this option to fetch webpages which are accessible only to logged-in users or JavaScript-powered websites.

```{attention}
Note that some JavaScript-powered websites actively delay loading of some content until the website is physically displayed on the screen. These stubborn websites will likely not work with this feature as this feature uses special "hidden" web browser to load the content.
```

## Dataflow
After your source feed data is downloaded either via URL or custom script, you can optionally post-process it with one more custom script, which will take **raw source data as input**. It must produce valid feed data to standard output while printing all error messages to error output.

Here is little flowchart explaining where and when scripts are used:

```{mermaid}
flowchart TB
  src{{"What kind of source was used?"}}
  url["Download the (feed) data from given URL"]
  scr["Generate the (feed) data with given script"]
  pstd{{"Is any post-process script set?"}}
  pst["Take previously obtained data and feed it to post-process script"]
  fin["Handover resulting feed data to RSS Guard for more processing - saving to DB etc."]

  src-->|URL|url
  src-->|Script|scr
  url-->pstd
  scr-->pstd
  pstd-->|Yes|pst
  pstd-->|No|fin
  pst-->fin
```

Typical post-processing filter might do things like CSS formatting, localization of content to another language, downloading of complete articles, some kind of filtering, or removing ads.

It's completely up to you if you decide to only use script as `Source` of the script or separate your custom functionality between `Source` script and `Post-process` script. Sometimes you might need different `Source` scripts for different online sources and the same `Post-process` script and vice versa.

## Example Scrapers
There are [examples of website scrapers](https://github.com/martinrotter/rssguard/tree/master/resources/scripts/scrapers). Most of them are written in Python 3, so their execution line is similar to `python "script.py"`. Make sure to examine each script for more information on how to use it.

## 3rd-party Tools
Third-party tools for scraping made to work with RSS Guard:
* [CSS2RSS](https://github.com/Owyn/CSS2RSS) - can be used to scrape websites with CSS selectors.
* [RSSGuardHelper](https://github.com/pipiscrew/RSSGuardHelper) - another CSS selectors helper.

Make sure to give credit to authors that they deserve.