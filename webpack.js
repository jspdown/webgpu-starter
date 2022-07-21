const path = require("path")
const assert = require("assert").strict
const HtmlWebpackPlugin = require("html-webpack-plugin")

module.exports = (env) => {
    assert(env.OUTPUT_DIR, "OUTPUT_DIR environment variable is not defined")

    const rootDir = process.cwd()
    const outputDir = path.resolve(rootDir, env.OUTPUT_DIR)
    const sourceDir = path.resolve(rootDir, "src/target/web")

    return {
        mode: "development",
        entry: {
            main: path.resolve(sourceDir, "index.ts"),
        },
        output: {
            filename: "bundle.js",
            path: outputDir,
        },
        resolve: {
            extensions: [".ts", ".js"],
            modules: [
                outputDir,
                "node_modules"
            ],
        },
        devtool: "source-map",
        module: {
            rules: [
                {
                    test: /\.css$/,
                    use: [
                        "style-loader",
                        {
                            loader: "css-loader",
                            options: {
                                url: true,
                                import: true,
                                modules: false,
                            },
                        },
                    ],
                },
            ],
        },
        plugins: [
            new HtmlWebpackPlugin({
                template: path.resolve(sourceDir, "index.html"),
                filename: "index.html",
            }),
        ],
    }
}
