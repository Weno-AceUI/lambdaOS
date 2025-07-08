## LambdaOS Developer Documentation

On LambdaOS, we use primarily use C++ for many of our backend and UI components. However we do not utilize Qt or other open source frameworks, instead we use a custom graphics API called LambdaGraphics, also written in C++. 

# Drawing UI components and visuals using LambdaGraphics

In LambdaGraphics, you draw UI, and structure them together, like how you would develop web applications using HTML. LambdaGraphics is **NOT** similar to HTML or related to web development in any way possible. 

LambdaGraphics can sometimes be a headache to write in. Hopefully this official documentation helps.

Drawing rectangles in LambdaGraphics is easy for many people who are used to C++'s syntax. To make a basic rectangle you can write this snippet of code:

``` cpp
ctx.drawRect(x + width/2 - 150, y + height/2 - 100, 300, 200, 0xFFFFFFFF);
```
This code draws a rectangle with the width defined with X and height defined with Y. The rectangle also has a background color of full white.

In LambdaGraphics background colors for main app windows or main UI components, are also definied using ``drawRect``, for example, if you were adding a background color to your lambdaOS application, you would write:

```cpp
ctx.drawRect(x, y, width, height, 0xFF222A36);
```

Notice that you didn't have to add or get specific values for width and height using x and y coordinates.

LambdaGraphics also supports other important UI components, including text. Drawing text in LambdaGraphics is simple and similar to how we did with DrawRect.
To draw Text in LambdaGraphics you can write this snippet of code:

```cpp
ctx.drawText(x + width/2 - 120, y + height/2 - 60, "Username:", 0xFF000000);
```

We used similar syntax in drawRect for drawText, as well as obviously the Content the drawText element shows.

Finally, you'll need a screen container to contain those drawable elements. In LambdaGraphics you can declare screen containers like this:

```cpp
void LoginScreen::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
  ```