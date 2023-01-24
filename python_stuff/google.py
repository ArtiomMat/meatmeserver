from icrawler.builtin import GoogleImageCrawler

google_Crawler = GoogleImageCrawler(storage = {'root_dir': r'sex'})
filters = dict(
    type='face')

google_Crawler.crawl(keyword = 'fat sex offender', max_num = 1000, filters=filters)