import { viteBundler } from '@vuepress/bundler-vite'
import { defaultTheme } from '@vuepress/theme-default'
import { defineUserConfig } from 'vuepress'

export default defineUserConfig({
  lang: 'en-US',
  title: 'Stratos Programming Language',
  description: 'Modern, safe, and expressive programming language with powerful type system and native async support',

  bundler: viteBundler(),

  theme: defaultTheme({
    logo: '/images/logo.svg',

    navbar: [
      {
        text: 'Guide',
        link: '/guide/',
      },
      {
        text: 'Examples',
        link: '/examples/',
      },
      {
        text: 'Reference',
        children: [
          {
            text: 'CLI Reference',
            link: '/reference/cli',
          },
          {
            text: 'Standard Library',
            link: '/reference/stdlib',
          },
        ],
      },
      {
        text: 'Community',
        link: '/community/',
      },
      {
        text: 'GitHub',
        link: 'https://github.com/stratos-lang/stratos',
      },
    ],

    sidebar: {
      '/guide/': [
        {
          text: 'Introduction',
          children: [
            '/guide/README.md',
            '/guide/getting-started.md',
            '/guide/installation.md',
          ],
        },
        {
          text: 'Language Features',
          children: [
            '/guide/basics.md',
            '/guide/control-flow.md',
            '/guide/functions.md',
            '/guide/null-safety.md',
            '/guide/oop.md',
            '/guide/packages.md',
          ],
        },
        {
          text: 'Advanced Topics',
          children: [
            '/guide/async.md',
            '/guide/error-handling.md',
            '/guide/dependency-management.md',
          ],
        },
      ],
      '/examples/': [
        {
          text: 'Code Examples',
          children: [
            '/examples/README.md',
            '/examples/basics.md',
            '/examples/control-flow.md',
            '/examples/functions.md',
            '/examples/null-safety.md',
            '/examples/oop.md',
            '/examples/packages.md',
          ],
        },
      ],
      '/reference/': [
        {
          text: 'Reference',
          children: [
            '/reference/cli.md',
            '/reference/stdlib.md',
          ],
        },
      ],
      '/community/': [
        {
          text: 'Community',
          children: [
            '/community/README.md',
            '/community/contributing.md',
            '/community/help.md',
            '/community/license.md',
          ],
        },
      ],
    },

    repo: 'stratos-lang/stratos',
    docsDir: 'docs/src',
    editLink: true,
    editLinkText: 'Edit this page on GitHub',
    lastUpdated: true,
    contributors: true,
  }),
})
