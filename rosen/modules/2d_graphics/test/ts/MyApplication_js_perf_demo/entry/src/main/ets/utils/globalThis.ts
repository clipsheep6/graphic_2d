import common from '@ohos.app.ability.common'
import image from '@ohos.multimedia.image'

export default class globalThis {
  private constructor() {}
  private static instance:globalThis
  private _uiContexts = new Map<string, common.UIAbilityContext>()
  private _pixelMap: image.PixelMap

  public static getInstance(): globalThis {
    if (!globalThis.instance) {
      globalThis.instance = new globalThis()
    }
    return globalThis.instance
  }

  getContext(key: string): common.UIAbilityContext | undefined {
    return this._uiContexts.get(key)
  }

  setContext(key: string, value: common.UIAbilityContext): void {
    this._uiContexts.set(key, value)
  }

  setPixelMap(pixelMap: image.PixelMap): void {
    this._pixelMap = pixelMap
  }

  getPixelMap(): image.PixelMap {
    return this._pixelMap
  }

}